//
// GOSSMerger - Merge CSV outputs from multiple threads
//

#include "GOSSMerger.hh"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GOSSMerger::MergeThreadOutputs(const std::string& directory)
{
  std::string dir = directory.empty() ? "." : directory;
  
  G4cout << "\n===================================================" << G4endl;
  G4cout << "  GOSS Merger: Combining thread outputs..." << G4endl;
  G4cout << "===================================================" << G4endl;
  
  // Find all CSV files
  auto csvFiles = FindCSVFiles(dir);
  
  if (csvFiles.empty()) {
    G4cout << "  No CSV files found to merge." << G4endl;
    return;
  }
  
  G4cout << "  Found " << csvFiles.size() << " CSV files to merge." << G4endl;
  
  // Accumulate data from all files
  std::map<int, DetectorData> mergedData;
  
  for (const auto& file : csvFiles) {
    G4cout << "  Processing: " << file << G4endl;
    ParseAndAccumulate(file, mergedData);
  }
  
  G4cout << "  Total detectors: " << mergedData.size() << G4endl;
  
  // Export merged results
  ExportMerged(mergedData, dir);
  
  G4cout << "===================================================" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<std::string> GOSSMerger::FindCSVFiles(const std::string& directory)
{
  std::vector<std::string> csvFiles;
  
  try {
    for (const auto& entry : fs::directory_iterator(directory)) {
      if (entry.is_regular_file()) {
        std::string filename = entry.path().filename().string();
        std::string extension = entry.path().extension().string();
        
        // Match GOSS output files: *_nt_seed_*_t*.csv
        if (extension == ".csv" && 
            filename.find("_nt_") != std::string::npos &&
            filename.find("_t") != std::string::npos) {
          csvFiles.push_back(entry.path().string());
        }
      }
    }
  } catch (const std::exception& e) {
    G4cerr << "  Error scanning directory: " << e.what() << G4endl;
  }
  
  // Sort for consistent processing
  std::sort(csvFiles.begin(), csvFiles.end());
  
  return csvFiles;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GOSSMerger::ParseAndAccumulate(const std::string& filepath,
                                     std::map<int, DetectorData>& data)
{
  std::ifstream file(filepath);
  if (!file.is_open()) {
    G4cerr << "  Error opening file: " << filepath << G4endl;
    return;
  }
  
  std::string line;
  bool headerSkipped = false;
  
  while (std::getline(file, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') continue;
    
    // Skip header line (contains column names)
    if (!headerSkipped) {
      // Check if this looks like a header (contains text like "Detector")
      if (line.find("Detector") != std::string::npos || 
          line.find("x_cm") != std::string::npos) {
        headerSkipped = true;
        continue;
      }
    }
    
    std::stringstream ss(line);
    std::string token;
    std::vector<double> values;
    
    // Parse CSV values
    while (std::getline(ss, token, ',')) {
      try {
        values.push_back(std::stod(token));
      } catch (...) {
        // Skip non-numeric values
        continue;
      }
    }
    
    // Expected columns: Detector_Number, x_cm, y_cm, z_cm, Total_Dose_Gy,
    //                   Dose_Per_Particle_Gy, Dose_Squared_Sum, 
    //                   Mean_Dose_Squared_Gy2, Uncertainty_3sigma_Per_Particle_Gy, nEvents
    if (values.size() >= 10) {
      int detNum = static_cast<int>(values[0]);
      
      // Initialize or accumulate
      if (data.find(detNum) == data.end()) {
        data[detNum].x_cm = values[1];
        data[detNum].y_cm = values[2];
        data[detNum].z_cm = values[3];
      }
      
      // Accumulate summable values
      data[detNum].totalDose += values[4];       // Total_Dose_Gy
      data[detNum].doseSquaredSum += values[6];  // Dose_Squared_Sum
      data[detNum].nEvents += static_cast<long>(values[9]); // nEvents
    }
  }
  
  file.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GOSSMerger::ExportMerged(const std::map<int, DetectorData>& data,
                               const std::string& directory)
{
  std::string outputPath = directory + "/goss_sd_dose_merged.csv";
  
  std::ofstream file(outputPath);
  if (!file.is_open()) {
    G4cerr << "  Error creating merged output file: " << outputPath << G4endl;
    return;
  }
  
  // Write header
  file << "Detector_Number,x_cm,y_cm,z_cm,Total_Dose_Gy,Dose_Per_Particle_Gy,Uncertainty_3sigma_Per_Particle_Gy,nEvents\n";
  
  // Write data with recalculated statistics
  for (const auto& pair : data) {
    int detNum = pair.first;
    const DetectorData& d = pair.second;
    
    double dosePerParticle = 0;
    double threeSigma = 0;
    
    if (d.nEvents > 0) {
      dosePerParticle = d.totalDose / d.nEvents;
      
      // 3sigma of dose per particle = 3 * sqrt((sum(D^2)/n - (sum(D)/n)^2) / n)
      double meanDoseSquared = d.doseSquaredSum / d.nEvents;
      double meanDose = d.totalDose / d.nEvents;
      double variance = meanDoseSquared - (meanDose * meanDose);
      
      if (variance > 0) {
        threeSigma = 3.0 * std::sqrt(variance / d.nEvents);
      }
    }
    
    file << detNum << ","
         << d.x_cm << ","
         << d.y_cm << ","
         << d.z_cm << ","
         << d.totalDose << ","
         << dosePerParticle << ","
         << threeSigma << ","
         << d.nEvents << "\n";
  }
  
  file.close();
  
  G4cout << "  Merged output saved to: " << outputPath << G4endl;
}
