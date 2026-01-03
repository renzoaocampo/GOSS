//
// GOSSMerger - Merge CSV outputs from multiple threads
//

#ifndef GOSSMerger_h
#define GOSSMerger_h 1

#include "globals.hh"
#include <string>
#include <vector>
#include <map>

/// Utility class to merge CSV output files from multiple threads
/// 
/// At the end of a multi-threaded run, each thread produces its own CSV file.
/// This class combines them into a single merged result file with:
/// - Summed dose values
/// - Recalculated dose per particle
/// - Recalculated 3-sigma uncertainty
///
/// Output file: goss_sd_dose_merged.csv

class GOSSMerger
{
public:
  /// Merge all CSV files matching the pattern in the current directory
  /// @param directory Directory containing the CSV files (empty = current)
  static void MergeThreadOutputs(const std::string& directory = "");

private:
  /// Data structure for a single detector
  struct DetectorData {
    double x_cm = 0;
    double y_cm = 0;
    double z_cm = 0;
    double totalDose = 0;
    double doseSquaredSum = 0;
    long nEvents = 0;
  };

  /// Find all thread CSV files in directory
  static std::vector<std::string> FindCSVFiles(const std::string& directory);
  
  /// Parse a single CSV file and accumulate data
  static void ParseAndAccumulate(const std::string& filepath,
                                  std::map<int, DetectorData>& data);
  
  /// Recalculate statistics and export merged file
  static void ExportMerged(const std::map<int, DetectorData>& data,
                           const std::string& directory);
};

#endif
