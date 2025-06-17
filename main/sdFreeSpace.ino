float sdFreeSpace() {
  uint32_t totalClusters = SD.vol()->clusterCount();
  uint32_t blocksPerCluster = SD.vol()->sectorsPerCluster();
  uint32_t totalBlocks = totalClusters * blocksPerCluster;
  uint64_t totalBytes = (uint64_t)totalBlocks * 512ULL;

  uint32_t freeClusters = SD.vol()->freeClusterCount();
  uint32_t freeBlocks = freeClusters * blocksPerCluster;
  uint64_t freeBytes = (uint64_t)freeBlocks * 512ULL;

  float percentFree = 100.0 * ((float)freeBytes / (float)totalBytes);
/*
  Serial.print("SD Total: ");
  Serial.print(totalBytes / (1024.0 * 1024.0));
  Serial.println(" MB");

  Serial.print("SD Free: ");
  Serial.print(freeBytes / (1024.0 * 1024.0));
  Serial.println(" MB");

  Serial.print("SD Free Percent: ");
  Serial.print(SDpercentFree, 2);
  Serial.println(" %");
  */
  return percentFree;
}