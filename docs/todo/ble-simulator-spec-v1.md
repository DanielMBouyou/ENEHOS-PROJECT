# ENEHOS - BLE Simulator Spec (V1)

Statut: `TODO (apres validation GNSS)`

Objectif:
- Simuler une sortie BLE depuis le XIAO pour fournir a l'equipe software un flux stable `GNSS + IMU`.
- Permettre le dev mobile sans attendre l'integration finale capteurs.

## 1) Perimetre V1

- Role BLE du XIAO: `Peripheral`
- Role smartphone/app: `Central`
- Transport: `BLE Notify` pour la navette mesures
- Donnees envoyees: dernier etat GNSS + IMU dans une trame unique

## 2) GATT (propose V1)

- Service custom ENEHOS:
  - UUID: `19B10010-E8F2-537E-4F6C-D104768A1214`
- Characteristic mesures (Notify + Read):
  - UUID: `19B10011-E8F2-537E-4F6C-D104768A1214`
  - Proprietes: `READ`, `NOTIFY`
  - Longueur fixe: `32 bytes`
- Characteristic commande (option V1.1, pas obligatoire tout de suite):
  - UUID: `19B10012-E8F2-537E-4F6C-D104768A1214`
  - Proprietes: `WRITE`, `WRITE_WO_RESP`

## 3) Payload binaire V1 (32 bytes)

Endian: `little-endian`

```
byte 0      : version (uint8) = 1
byte 1      : flags (uint8)
              bit0 gnss_fix_valid
              bit1 imu_valid
              bit2 gnss_fresh
              bit3 reserved
bytes 2-3   : seq (uint16) increment a chaque notify
bytes 4-7   : t_ms (uint32) uptime device en ms
bytes 8-11  : lat_e7 (int32) latitude * 1e7
bytes 12-15 : lon_e7 (int32) longitude * 1e7
bytes 16-17 : speed_cms (uint16) vitesse cm/s
bytes 18-19 : hdop_centi (uint16) hdop * 100
byte 20     : sats (uint8) nb satellites
bytes 21-22 : roll_cdeg (int16) roll deg * 100
bytes 23-24 : pitch_cdeg (int16) pitch deg * 100
bytes 25-26 : heading_cdeg (uint16) heading deg * 100 (0..36000)
bytes 27-28 : battery_mV (uint16) batterie en mV
byte 29     : status_code (uint8) 0=OK, 1=GNSS_LOST, 2=IMU_LOST, 3=DEGRADED
bytes 30-31 : crc16_ccitt (uint16)
```

## 4) Cadence et contraintes

- IMU interne: `20 Hz` (BNO rotation vector)
- Trame BLE envoyee: `10 Hz` (notify toutes les 100 ms)
- GNSS natif: `1 Hz` ; la derniere valeur GNSS est republiee dans chaque trame 10 Hz
- Sequence `seq` obligatoire pour detecter les pertes de notify cote app

## 5) Contrat pour l'equipe software

- Decoder la trame 32 bytes selon ce document
- Verifier `version == 1`, puis `crc16`
- Utiliser `seq` pour detecter les trous
- Afficher:
  - cap (heading)
  - fix GNSS + lat/lon
  - qualite (`hdop`, `sats`, `status_code`)
- Si `gnss_fix_valid=0`, continuer a traiter IMU sans bloquer l'UI

## 6) Scenarios de test BLE (avant handover software)

- T1: notification stable 10 Hz pendant 5 min
- T2: perte de fix GNSS -> `flags/status_code` coherents
- T3: redemarrage XIAO -> reprise notify + `seq` repart de 0
- T4: deconnexion/reconnexion smartphone -> notify repart automatiquement
- T5: test charge utile:
  - 20 bytes (mode compact)
  - 32 bytes (mode V1 cible)
  - 64 bytes (stress simple)

## 7) Plan TODO (apres tests GNSS)

1. Geler le mapping champs GNSS definitif (source parser L86).
2. Implementer `NavetteMesuresV1` dans `firmware/shared/`.
3. Ajouter calcul `crc16_ccitt`.
4. Publier notify 10 Hz depuis le firmware boitier.
5. Ecrire un petit decodeur de reference Python pour l'equipe software.
6. Documenter un exemple hex + JSON decode dans `docs/protocol/`.

## 8) Exemple decode (indicatif)

```
version=1
seq=1042
t_ms=123456
lat=48.8566000
lon=2.3522000
speed_mps=0.82
hdop=0.93
sats=10
roll=1.24
pitch=-3.11
heading=274.56
battery=3920 mV
status=OK
```

