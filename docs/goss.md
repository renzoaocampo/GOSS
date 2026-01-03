# GOSS Dose Scoring

GOSS (Geant4 Orchestrated Scoring Suite) includes configurable geometry and dose scoring.

## Command Reference

### GOSS Commands (`/goss/...`)

| Command | Description | Default |
|---------|-------------|---------|
| `/goss/saveInterval <N>` | Save dose output every N events | 1000000 |
| `/goss/outputFile <name>` | Output file name (without extension) | output |
| `/goss/seed <value>` | Random seed (auto if not set) | auto |

### World Geometry (`/my_geom/...`)

| Command | Description | Default |
|---------|-------------|---------|
| `/my_geom/worldXY <value>` | World **half-size** in X and Y | 50 cm |
| `/my_geom/worldZ <value>` | World **half-size** in Z | 100 cm |

### Phantom Configuration (`/my_geom/phantom/...`)

| Command | Description | Default |
|---------|-------------|---------|
| `/my_geom/phantom/material <name>` | NIST material name | G4_WATER |
| `/my_geom/phantom/halfSizeX <value>` | Phantom **half-size** X | 15 cm |
| `/my_geom/phantom/halfSizeY <value>` | Phantom **half-size** Y | 15 cm |
| `/my_geom/phantom/halfSizeZ <value>` | Phantom **half-size** Z (depth/2) | 20 cm |
| `/my_geom/phantom/positionZ <value>` | Phantom **center** Z position | 0 cm |

> [!NOTE]
> Full phantom size = 2 × halfSize. Example: `halfSizeX 15 cm` → 30 cm total width.

### Detector Grid (`/my_geom/detector/...`)

| Command | Description | Default |
|---------|-------------|---------|
| `/my_geom/detector/material <name>` | NIST material name | G4_Si |
| `/my_geom/detector/halfSizeX <value>` | Detector **half-size** X | 0.1325 cm |
| `/my_geom/detector/halfSizeY <value>` | Detector **half-size** Y | 0.1325 cm |
| `/my_geom/detector/halfThickness <value>` | Detector **half-thickness** (Z) | 0.03 cm |
| `/my_geom/detector/gridSize <N>` | Detectors per row AND column | 40 |
| `/my_geom/detector/spacing <value>` | **Center-to-center** spacing | 0.8 cm |
| `/my_geom/detector/numLayers <N>` | Number of layers | 5 |
| `/my_geom/detector/layerSpacing <value>` | Z distance between layers | 2 cm |
| `/my_geom/detector/firstLayerZ <value>` | Z position of first layer | 15 cm |

> [!IMPORTANT]
> All geometry commands must be placed **before** `/run/initialize`

---

## Example Macro

```bash
#================================================
# Phantom Configuration
#================================================
/my_geom/phantom/material    G4_WATER
/my_geom/phantom/halfSizeX   15 cm
/my_geom/phantom/halfSizeY   15 cm
/my_geom/phantom/halfSizeZ   20 cm
/my_geom/phantom/positionZ   10 cm

#================================================
# Detector Grid Configuration
#================================================
/my_geom/detector/material       G4_Si
/my_geom/detector/halfSizeX      0.1325 cm
/my_geom/detector/halfSizeY      0.1325 cm
/my_geom/detector/halfThickness  0.03 cm
/my_geom/detector/gridSize       40
/my_geom/detector/spacing        0.8 cm
/my_geom/detector/numLayers      5
/my_geom/detector/layerSpacing   2 cm
/my_geom/detector/firstLayerZ    15 cm

#================================================
# GOSS Dose Scoring
#================================================
/goss/saveInterval  1000000
/goss/outputFile    dose_results

/run/initialize

# GPS configuration
/gps/particle e-
/gps/energy 6 MeV
# ...

/run/beamOn 10000000
```

---

## Output Format

CSV file with columns:

| Column | Description | Units |
|--------|-------------|-------|
| Detector_Number | Copy number | - |
| x_cm, y_cm, z_cm | Position | cm |
| Total_Dose_Gy | Accumulated dose | Gy |
| Dose_Per_Particle_Gy | Dose per primary | Gy |
| Dose_Squared_Sum | Σ(D²) | Gy² |
| Mean_Dose_Squared_Gy2 | ⟨D²⟩ | Gy² |
| Uncertainty_3sigma_Gy | 3σ uncertainty | Gy |
| nEvents | Events processed | - |

---

## Common NIST Materials

| Material Name | Description |
|--------------|-------------|
| `G4_WATER` | Liquid water |
| `G4_PMMA` | Acrylic/Lucite |
| `G4_TISSUE_SOFT_ICRP` | Soft tissue |
| `G4_BONE_COMPACT_ICRU` | Bone |
| `G4_Si` | Silicon |
| `G4_Ge` | Germanium |
| `G4_AIR` | Air |

---

## Automatic CSV Merging

When running with multiple threads, each thread produces its own CSV file:

```
dose_output_nt_seed_12345_t0.csv
dose_output_nt_seed_12345_t1.csv
dose_output_nt_seed_12345_t2.csv
...
```

At the end of the run, GOSS **automatically merges** these into a single file:

```
goss_sd_dose_merged.csv
```

### Merged Output Columns

| Column | Description |
|--------|-------------|
| Detector_Number | Detector ID |
| x_cm, y_cm, z_cm | Position |
| Total_Dose_Gy | Combined dose from all threads |
| Dose_Per_Particle_Gy | Recalculated average |
| Uncertainty_3sigma_Gy | Recalculated 3σ uncertainty |
| nEvents | Total events from all threads |

> [!NOTE]
> The merge happens automatically in the master thread at the end of the run.
> No manual post-processing is required.
