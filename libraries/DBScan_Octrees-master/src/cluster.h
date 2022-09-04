/**
 *@file cluster.h
 *Cluster for 3d points.
 */

#ifndef CLUSTER
#define CLUSTER

#include "HTRBasicDataStructures.h"
#include "OctreeGenerator.h"

namespace dbScanSpace {
class cluster {
 public:
  vector<pcl::mod_pointXYZ> clusterPoints;
  vector<htr::Point3D> clusterPoints3D;

  pcl::mod_pointXYZ centroid;
  htr::Point3D centroid3D;
  bool visited;

  cluster();
  void calculateCentroid();
  void toPoint3D();

 private:
};
}  // namespace dbScanSpace

#endif  // CLUSTER