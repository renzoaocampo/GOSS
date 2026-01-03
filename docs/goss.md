# GOSS Dose Scoring

GOSS (Geant4 Orchestrated Scoring Suite) includes configurable geometry and dose scoring.

## Command Reference

### GOSS Commands (`/goss/...`)

| Command | Description | Default |
|---------|-------------|---------|
| `/goss/saveInterval <N>` | Save dose output every N events | 1000000 |
| `/goss/outputFile <name>` | Output file name (without extension) | output |
| `/goss/seed <value>` | Random seed (auto if not set) | auto |
| `/goss/mergeCSV <bool>` | Enable/disable automatic CSV merge | true |

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

## Dose Calculation Mathematics

### Per-Thread Calculations (Individual CSV)

For each detector and each event, the SensitiveDetector accumulates:

**1. Total Dose (Gy):**
$$D_{total} = \sum_{i=1}^{N} \frac{E_i}{m}$$

Where:

- $E_i$ = Energy deposited in event $i$ (Joules)
- $m$ = Detector mass (kg)
- $N$ = Total events

**2. Dose Per Particle (Gy):**
$$\bar{D} = \frac{D_{total}}{N}$$

**3. Dose Squared Sum (Gy²):**

For statistical uncertainty, we track the sum of squared per-event doses:
$$S = \sum_{i=1}^{N} d_i^2$$

Where $d_i = E_i / m$ is the dose deposited in event $i$.

**4. Mean Dose Squared (Gy²):**
$$\langle D^2 \rangle = \frac{S}{N}$$

**5. 3-Sigma Uncertainty (Gy):**

Using the standard error of the mean:
$$\sigma_{\bar{D}} = \sqrt{\frac{\langle D^2 \rangle - \bar{D}^2}{N}}$$

The 3σ uncertainty (99.7% confidence interval):
$$U_{3\sigma} = 3 \cdot \sigma_{\bar{D}} = 3 \sqrt{\frac{\langle D^2 \rangle - \bar{D}^2}{N}}$$

---

### Merged Calculations (Multi-Thread)

When merging CSV files from multiple threads, values are combined as follows:

**Summable quantities (directly added):**

- `Total_Dose_Gy`: $D_{merged} = \sum_{t} D_t$
- `Dose_Squared_Sum`: $S_{merged} = \sum_{t} S_t$
- `nEvents`: $N_{merged} = \sum_{t} N_t$

**Recalculated quantities:**

**1. Merged Dose Per Particle:**
$$\bar{D}_{merged} = \frac{D_{merged}}{N_{merged}}$$

**2. Merged 3-Sigma Uncertainty:**
$$U_{3\sigma,merged} = 3 \sqrt{\frac{S_{merged}/N_{merged} - (D_{merged}/N_{merged})^2}{N_{merged}}}$$

Expanding:
$$U_{3\sigma,merged} = 3 \sqrt{\frac{S_{merged} - D_{merged}^2 / N_{merged}}{N_{merged}^2}}$$

---

## Output Files

### Per-Thread CSV Columns

| Column | Symbol | Formula | Units |
|--------|--------|---------|-------|
| `Detector_Number` | - | Copy number | - |
| `x_cm, y_cm, z_cm` | $(x,y,z)$ | Position | cm |
| `Total_Dose_Gy` | $D_{total}$ | $\sum E_i / m$ | Gy |
| `Dose_Per_Particle_Gy` | $\bar{D}$ | $D_{total} / N$ | Gy |
| `Dose_Squared_Sum` | $S$ | $\sum d_i^2$ | Gy² |
| `Mean_Dose_Squared_Gy2` | $\langle D^2 \rangle$ | $S / N$ | Gy² |
| `Uncertainty_3sigma_Gy` | $U_{3\sigma}$ | $3\sqrt{(\langle D^2 \rangle - \bar{D}^2)/N}$ | Gy |
| `nEvents` | $N$ | Event count | - |

### Merged CSV Columns

| Column | Formula |
|--------|---------|
| `Total_Dose_Gy` | $\sum_t D_t$ |
| `Dose_Per_Particle_Gy` | $D_{merged} / N_{merged}$ |
| `Uncertainty_3sigma_Gy` | See merged formula above |
| `nEvents` | $\sum_t N_t$ |

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

Control via macro:

```bash
/goss/mergeCSV true   # Enable (default)
/goss/mergeCSV false  # Disable
```

When running with multiple threads, each thread produces its own CSV file:

```
dose_output_nt_seed_12345_t0.csv
dose_output_nt_seed_12345_t1.csv
...
```

At the end of the run, GOSS **automatically merges** these into:

```
goss_sd_dose_merged.csv
```

> [!NOTE]
> The merge happens automatically in the master thread at the end of the run.
