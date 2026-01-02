# GOSS: Geant4 Orchestrated Scoring Suite

<img width="1280" height="908" alt="GOOS (1)-Photoroom" src="https://github.com/user-attachments/assets/1ce0794d-b52e-4f01-8242-7f41c7b23111" />

GOSS is a lightweight orchestration suite for Geant4 that consolidates existing components into a structured, reproducible workflow. It is designed to simplify setup and reduce boilerplate while maintaining full flexibility for dose scoring and analysis.

---

## Key Features

| Feature | Description |
|---------|-------------|
| **IAEA Phase-Space Files** | Read and write PHSP files for particle sources with full MT support |
| **Sensitive Detectors** | Preconfigured detector grids with automatic dose scoring in CSV format |
| **Configurable Geometry** | Runtime configuration via UI commands for world and detector parameters |
| **Multi-threaded** | Full support for Geant4 MT builds with thread-safe operations |

---

## Project Structure

```
GOSS/
├── GOSS/                    # Main application source
│   ├── include/             # Header files
│   │   ├── G4IAEAphspReader.hh
│   │   ├── G4IAEAphspWriter.hh
│   │   ├── SensitiveDetector.hh
│   │   └── ...
│   ├── src/                 # Implementation files
│   │   ├── DetectorConstruction.cc
│   │   ├── SensitiveDetector.cc
│   │   └── ...
│   ├── iaea_phsp/           # IAEA phase-space library
│   ├── phsp/                # Example PHSP files
│   ├── CMakeLists.txt
│   └── IAEAphsp.cc          # Main application entry
├── docs/                    # Documentation
│   └── phsp.md              # Detailed PHSP documentation
├── LICENSE
└── README.md
```

---

## Components

### 1. IAEA Phase-Space Reader/Writer

Supports reading and writing IAEA-format phase-space files:

- **Reader**: Load particles from `.IAEAphsp` files as primary generator
- **Writer**: Record particles at constant-Z scoring planes
- Full support for gantry/collimator rotations and spatial transforms

```bash
# UI Commands
/action/IAEAphspReader/fileName <name>       # Read from <name>.IAEA* files
/action/IAEAphspWriter/namePrefix <name>     # Write output files
/action/IAEAphspWriter/zphsp <z> <unit>      # Define scoring plane
```

📖 See [docs/phsp.md](docs/phsp.md) for complete PHSP documentation.

### 2. Sensitive Detector Grid

Configurable detector array for dose scoring:

- 40×40 detector grid with 5 layers
- Automatic energy deposition tracking
- CSV output with position, energy, and statistical data

**Output columns:**

- `Detector_Number`, `x`, `y`, `z`
- `Total_Energy_Deposited`, `Energy_Per_Particle`
- `Energy_Squared_Sum`, `Quadric_Energy_Per_Particle`
- `3sigma` (uncertainty), `nEventCounterThread`

### 3. Geometry Configuration

Runtime-configurable world and detector geometry:

```bash
/my_geom/worldXY <size> <unit>   # World XY half-size
/my_geom/worldZ <size> <unit>    # World Z half-size
```

---

## Requirements

- **Geant4** ≥ 10.6 (with MT support recommended)
- **CMake** ≥ 3.16
- C++17 compatible compiler

---

## Build

```bash
# Set Geant4 path
export Geant4_DIR=/path/to/geant4/lib/cmake/Geant4

# Build
cd GOSS
mkdir build && cd build
cmake -DGeant4_DIR="$Geant4_DIR" ..
cmake --build . --parallel
```

---

## Running

### Interactive Mode

```bash
./IAEAphsp
```

### Batch Mode (with macros)

```bash
./IAEAphsp test-reader.mac      # Read PHSP file only
./IAEAphsp test-writer.mac      # Write PHSP output
./IAEAphsp test-rw.mac          # Read and write PHSP
./IAEAphsp vis.mac              # Visualization
```

### Multi-threaded

```bash
./IAEAphsp test-reader.mac 4    # Run with 4 threads
```

---

## Output

Results are saved to `output.csv` in the working directory, containing dose data for each detector with statistical analysis.

---

## Documentation

- [PHSP Documentation](docs/phsp.md) — Detailed IAEA phase-space file usage

---

## License

This project is distributed under the **Apache License 2.0**. See [LICENSE](LICENSE) for details.

---

## Citation

If you use the IAEA phase-space components, please cite:

> M.A. Cortes-Giraldo et al., Int J Radiat Biol 88(1-2): 200-208 (2012)  
> [doi: 10.3109/09553002.2011.627977](https://doi.org/10.3109/09553002.2011.627977)
