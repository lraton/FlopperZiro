/*
 * Copyright (c) 2024, lraton
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// Returns the percentage of free space remaining on the SD card.
// Computed from FAT cluster counts provided by SdFat.
float getSdFreePercent() {
  uint32_t totalClusters   = SD.vol()->clusterCount();
  uint32_t sectorsPerClus  = SD.vol()->sectorsPerCluster();
  uint64_t totalBytes      = (uint64_t)(totalClusters * sectorsPerClus) * 512ULL;

  uint32_t freeClusters    = SD.vol()->freeClusterCount();
  uint64_t freeBytes       = (uint64_t)(freeClusters  * sectorsPerClus) * 512ULL;

  return 100.0f * ((float)freeBytes / (float)totalBytes);
}