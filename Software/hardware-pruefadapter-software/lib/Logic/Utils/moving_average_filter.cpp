/**
 * @file moving_average_filter.cpp
 * @brief Implementierung des gleitenden Mittelwert-Filters.
 */

#include "moving_average_filter.h"

namespace hardware_pruefadapter {
namespace utils {

void MovingAverageFilter::AddValue(std::uint16_t new_value) {
  buffer_[index_] = new_value;
  index_ = (index_ + 1) % config::kAdcFilterSamples;
  
  if (count_ < config::kAdcFilterSamples) {
    count_++;
  }
}

std::uint16_t MovingAverageFilter::GetAverage() const {
  if (count_ == 0) return 0;
  
  std::uint32_t sum = 0;
  for (std::uint8_t i = 0; i < count_; i++) {
    sum += buffer_[i];
  }
  
  return static_cast<std::uint16_t>(sum / count_);
}

void MovingAverageFilter::Reset() {
  count_ = 0;
  index_ = 0;
  // Der Puffer muss nicht zwingend mit 0 überschrieben werden, 
  // da 'count_' auf 0 steht und alte Werte ignoriert werden.
}

}  // namespace utils
}  // namespace hardware_pruefadapter