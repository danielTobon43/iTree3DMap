#include <pcl/visualization/pcl_visualizer.h>

void display_cloud(pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud) {
  // pcl visualizer object
  pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("PCL VISUALIZER"));

  // initial configuration
  viewer->setPosition(0, 0);
  viewer->setBackgroundColor(0.0, 0.0, 0.0, 0.0);
  viewer->setShowFPS(false);

  // add coordinate system
  viewer->addCoordinateSystem();

  // add x,y,z label to coordinate system
  pcl::PointXYZ p1, p2, p3;
  p1.getArray3fMap() << 1, 0, 0;
  p2.getArray3fMap() << 0, 1, 0;
  p3.getArray3fMap() << 0, 0.1, 1;
  viewer->addText3D("x", p1, 0.2, 1, 0, 0, "x_");
  viewer->addText3D("y", p2, 0.2, 0, 1, 0, "y_");
  viewer->addText3D("z", p3, 0.2, 0, 0, 1, "z_");

  // add points label
  std::string str = "Points: ";
  std::stringstream ss;
  ss << cloud->points.size();
  str += ss.str();
  int xpos = 1.0;
  int ypos = 1.0;
  int fontSize = 13;
  double r = 1.0;
  double g = 1.0;
  double b = 1.0;
  viewer->addText(str, xpos, ypos, fontSize, r, g, b, "text1");

  // set cloud color if not defined
  if (cloud->points[0].r <= 0 && cloud->points[0].g <= 0 && cloud->points[0].b <= 0) {
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> color_handler(cloud, 255, 255, 0);
    viewer->removeAllPointClouds(0);
    viewer->addPointCloud(cloud, color_handler, "POINTCLOUD");
  } else {
    viewer->addPointCloud(cloud, "POINTCLOUD");
  }

  // init camera view
  viewer->initCameraParameters();
  viewer->resetCamera();

  // help message
  pcl::console::print_info("\npress [q] to exit!\n");

  // display visualizer
  viewer->spin();
  viewer->close();
}