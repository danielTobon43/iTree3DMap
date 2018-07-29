﻿#include "include/Utilities.h"

std::string output_dir;
using namespace tinyxml2;

void Utilities::run_openMVG(){

  //COMMAND LINE INPUT
  std::string input_dir;
  std::string focal_length;
  std::string project_name;
  double n=-1;
  const std::string red("\033[0;31m");
  const std::string blue("\033[0;34m");
  const std::string yellow("\033[0;33m");
  const std::string reset("\033[0m");
  std::string answer;

  bool choise = false;

  while(output_dir.size()<=0 or input_dir.size()<=0 or focal_length.size() <=0){

    if(project_name.size()<=0){

      std::cout << blue << "\nEnter the project name:" << reset << std::endl;
      std::cout << "------------------------------------------" << std::endl;
      std::getline(std::cin, project_name);

    }

    if(not choise){

      std::cout << yellow << "project name = " << project_name << " are you sure? (yes/no)\n" << reset << std::endl;
      std::getline(std::cin, answer);
      if(answer == "yes"){
        std::cout << yellow << "Using project name as --> " << project_name << reset << std::endl;
        choise = true;
      }else if(answer == "no"){
        project_name.clear();
        choise = false;
        continue;
      }else{
        std::cout << red << answer << " is not a valid answer." << reset << std::endl;
        choise = false;
        project_name.clear();
        continue;
      }
    }

    if(input_dir.size()<=0){

         std::cout << blue << "\nEnter the images directorie path:" << reset << std::endl;
         std::cout << "------------------------------------------" << std::endl;
         std::getline(std::cin, input_dir);

         boost::filesystem::path dirPath(input_dir);

         if(not boost::filesystem::exists(dirPath) or not boost::filesystem::is_directory(dirPath)){
             std::cout << red << "Error. cannot open directory: " << input_dir << reset <<std::endl;
             input_dir.clear();
             continue;
         }

         bool images_files = true;

         for(boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(dirPath)){
               std::string extension = x.path().extension().string();
               boost::algorithm::to_lower(extension);
               if(extension == ".jpg" or extension == ".png"){
                 std::cout << yellow << "Found images file." << reset << std::endl;
                 break;
               }else{
                 images_files = false;
                 break;
               }
         }
         if(not images_files){
           std::cout << red << "Unable to find images files in directory (\"" << input_dir << "\")."
                     << reset <<std::endl;
           input_dir.clear();
           continue;
         }

         if(output_dir.size()<=0){

             std::cout << blue << "\nEnter the output directorie path:" << reset << std::endl;
             std::cout << "------------------------------------------" << std::endl;
             std::getline(std::cin, output_dir);

             boost::filesystem::path dirPath2(output_dir);

             if(not boost::filesystem::exists(dirPath2) or not boost::filesystem::is_directory(dirPath2)){
                 std::cout << red <<"Error. cannot found directory: " << output_dir << reset << std::endl;
                 output_dir.clear();
                 continue;
             }
         }

     }else if(output_dir.size()<=0){

         std::cout << blue << "\nEnter the output directorie path:" << reset << std::endl;
         std::cout << "------------------------------------------" << std::endl;
         std::getline(std::cin, output_dir);

         boost::filesystem::path dirPath2(output_dir);

         if(not boost::filesystem::exists(dirPath2) or not boost::filesystem::is_directory(dirPath2)){
             std::cout << red << "Cannot found directory: " << output_dir << reset << std::endl;
             output_dir.clear();
             continue;
         }
     }

     while(std::cout << blue << "\nEnter the focal length:\n" << reset <<
           "------------------------------------------" << std::endl
           && ! (std::cin >> n)){
             std::cin.clear();
             std::getline(std::cin, focal_length);
             std::cout << yellow << "I am sorry, but '" << focal_length << "' is not a number" << reset
                       << std::endl;
     }

     if(n<=0){
         std::cout << red << "Error: insert a valid focal length" << reset << std::endl;
         n = -1;
         focal_length.clear();
         continue;
     }

     std::ostringstream strs;
     strs << n;
     focal_length = strs.str();
     break;
 }

 int dont_care;
 std::string folder_name = "mkdir ";
 folder_name += output_dir;
 folder_name += "/";
 folder_name += project_name;

 dont_care = std::system(folder_name.c_str());

 output_dir += "/";
 output_dir += project_name;

 std::string output_pcd_files = "3D_Mapping";
 std::string folder_name2 = "mkdir ";
 folder_name2 += output_dir;
 folder_name2 += "/";
 folder_name2 += output_pcd_files;

 dont_care = std::system(folder_name2.c_str());

 std::string command = "python ";
 std::string openMVG = "~/catkin_ws/src/iTree3DMap/openMVG/openMVG_Build/software/SfM/SfM_SequentialPipeline.py ";

 command += openMVG;
 command += input_dir;
 command += " ";
 command += output_dir;
 command += " ";
 command += focal_length;

 std::cout << blue << "\n3D Mapping with openMVG initializing..." << reset << std::endl;
 dont_care = std::system(command.c_str());

 if(dont_care > 0){
  std::cout << red << "Failed. SfM_SequentialPipeline.py not found" << reset << std::endl;
  std::exit(-1);
 }

 std::cout << "\n" << "3D Mapping --> [COMPLETE]." << std::endl;

}

void Utilities::createPMVS_Files(){

  const std::string red("\033[0;31m");
  const std::string blue("\033[0;34m");
  const std::string reset("\033[0m");

  std::cout << "\n------------------------------------------" << std::endl;
  std::cout << blue <<"Creating files for PMVS2..." << reset << std::endl;

  std::string command2 = "~/catkin_ws/src/iTree3DMap/openMVG/openMVG_Build/Linux-x86_64-RELEASE/openMVG_main_openMVG2PMVS -i ";
  command2 += output_dir;
  command2 += "/reconstruction_sequential/sfm_data.bin -o ";
  command2 += output_dir;

  int dont_care = std::system(command2.c_str());

  if(dont_care > 0){
   std::cout << red << "Failed. openMVG_main_openMVG2PMVS not found" << reset << std::endl;
   std::exit(-1);
  }

}

void Utilities::densifyWithPMVS(pcl::PointCloud<pcl::PointXYZRGB>::Ptr& output_cloud){

  const std::string blue("\033[0;34m");
  const std::string reset("\033[0m");

  std::cout << "\n------------------------------------------" << std::endl;
  std::cout << blue << "Densify cloud process initializing..." << reset << std::endl;

  std::string command3 = "~/catkin_ws/src/iTree3DMap/programs/pmvs2 ";
  command3 += output_dir;
  command3 += "/PMVS/ ";
  command3 += "pmvs_options.txt";

  int dont_care = std::system("chmod 771 ~/catkin_ws/src/iTree3DMap/programs/pmvs2");
  dont_care = std::system(command3.c_str());

  if(dont_care > 0){
   std::cout << "Failed. ./pmvs2 no found" << std::endl;
   std::exit(-1);
  }

  std::string cloudPLY = output_dir;
  cloudPLY += "/PMVS/models/pmvs_options.txt.ply";

  pcl::PLYReader inputPlyCloud;
  inputPlyCloud.read(cloudPLY,*output_cloud);

  std::cout << "Densify proccess --> [OK]" << std::endl;
  std::cout << "Saving dense 3d mapping file with prefix --> MAP3D_dense.pcd" << std::endl;

  std::string prefix1 = output_dir;
  std::string output_pcd_files = "3D_Mapping";
  prefix1 += "/";
  prefix1 += output_pcd_files;
  prefix1 += "/";
  prefix1 += "MAP3D_dense.pcd";

  pcl::io::savePCDFileBinary(prefix1.c_str(), *output_cloud);

  std::cout << "\n------------------------------------------" << std::endl;
  std::cout << "Showing 3D mapping" << std::endl;

  pcl::visualization::PCLVisualizer viewer = pcl::visualization::PCLVisualizer("MAP3D",true);

  viewer.setPosition(0,0);
  viewer.setSize(640,480);
  viewer.setBackgroundColor(0.05, 0.05, 0.05, 0); // Setting background to a dark grey
  viewer.addCoordinateSystem ();
  viewer.setCameraPosition(0,0,1,0,0,0);
  pcl::PointXYZ p1, p2, p3;
  p1.getArray3fMap() << 1, 0, 0;
  p2.getArray3fMap() << 0, 1, 0;
  p3.getArray3fMap() << 0,0.1,1;

  viewer.addText3D("x", p1, 0.2, 1, 0, 0, "x_");
  viewer.addText3D("y", p2, 0.2, 0, 1, 0, "y_");
  viewer.addText3D ("z", p3, 0.2, 0, 0, 1, "z_");
  viewer.addPointCloud(output_cloud,"tree_cloud");
  viewer.resetCamera();

  std::cout << "Press [q] to continue --> SEGMENTATION PROCESS!" << std::endl;

  while(!viewer.wasStopped ()) {
         viewer.spin();
  }

}

// This function displays the help
void Utilities::help(){
  const std::string green("\033[0;32m");
  const std::string reset("\033[0m");
  std::cout << green << "===============================================\n"
               "The program create a 3D mapping of individual tree from a images sequence using openMVG and PMVS2."
            << std::endl << "Enter:\n" << "<project name> and press \"Enter\"." << "\n"
            << "<path image sequence> and press \"Enter\"." << "\n"
            << "<output path> and press \"Enter\"." << std::endl
            << "<focal length> and press \"Enter\"." << std::endl
            << "<scale factor> and press \"Enter\"." << std::endl               
            << "==============================================="<< reset << std::endl
            << "How to get the scale factor?" << std::endl
            << "Enter the image that contain the pattern and press \"Enter\"." << std::endl;
}

void Utilities::uniformScaling(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
                               pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_scaled,const float scale,
                               bool show){

  std::cout << "Scaling pointcloud to real measurements..." << std::endl;

  Eigen::MatrixXf scale_matrix(4,4);
                                            //Uniform scaling: vx = vy = vz = s --> Common scale factor
  scale_matrix << scale,  0,    0,    0,    //       |vx  0   0   0|
                   0,   scale,  0,    0,    //  Sv = |0   vy  0   0| => Scale matrix
                   0,     0,  scale,  0,    //       |0   0   vz  0|
                   0,     0,    0,    1;    //       |0   0   0   1|
                                            //https://en.wikipedia.org/wiki/Scaling_(geometry)

  std::cout << "Here is the matrix scale factor:\n" << scale_matrix << std::endl;

  std::cout << "Executing the transformation..." << std::endl;
  pcl::transformPointCloud(*cloud, *cloud_scaled, scale_matrix);

  if(show){

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_xyz (new pcl::PointCloud<pcl::PointXYZ>());
    pcl::copyPointCloud(*cloud,*cloud_xyz);

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_scale_xyz (new pcl::PointCloud<pcl::PointXYZ>());
    pcl::copyPointCloud(*cloud_scaled,*cloud_scale_xyz);

    // Visualization
    pcl::visualization::PCLVisualizer viewer = pcl::visualization::PCLVisualizer("cloud scale",true);
    viewer.setPosition(0,0);
    viewer.setSize(640,480);
    viewer.setBackgroundColor(0.05, 0.05, 0.05, 0);
    viewer.setCameraPosition(0,0,1,0,0,0);

    /*Coordinate system*/
    viewer.addCoordinateSystem ();

    pcl::PointXYZ p1, p2, p3;
    p1.getArray3fMap() << 1, 0, 0;
    p2.getArray3fMap() << 0, 1, 0;
    p3.getArray3fMap() << 0,0.1,1;

    viewer.addText3D("x", p1, 0.2, 1, 0, 0, "x_");
    viewer.addText3D("y", p2, 0.2, 0, 1, 0, "y_");
    viewer.addText3D ("z", p3, 0.2, 0, 0, 1, "z_");

    // White --> original cloud
    viewer.addPointCloud(cloud_xyz, "original_cloud");

    // Red --> cloud scale
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud_scale_color(cloud_scale_xyz, 230, 20, 20);
    viewer.addPointCloud(cloud_scale_xyz,cloud_scale_color, "transformed_cloud");

    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "original_cloud");
    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "transformed_cloud");

    viewer.resetCamera();

    std::cout << "Press [q] to continue process segmentation!" << std::endl;

    while(!viewer.wasStopped ()) {
           viewer.spin();
    }

    viewer.close();
  }
}

void Utilities::getScaleFactor(float& scale_factor){

  std::string img_path;
  const std::string red("\033[0;31m");
  const std::string blue("\033[0;34m");
  const std::string yellow("\033[0;33m");
  const std::string reset("\033[0m");

  std::cout << "Enter image pattern path." << std::endl;
  std::cout << "------------------------------------------" << std::endl;

  std::getline(std::cin, img_path);

  cv::Mat img = cv::imread(img_path.c_str(),1);
  cv::Mat img_copy = img.clone();

  cv::Mat gray;

  cv::cvtColor(img_copy,gray,CV_BGR2GRAY);
  cv::GaussianBlur(gray,gray,cv::Size(9,9),2,2);
  //cv::medianBlur(gray,gray,5);

  std::vector<cv::Vec3f> circles;

  cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0);
  cv::Point pattern_center;

  for(size_t i = 0; i < circles.size(); i++ ){

    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    pattern_center = cv::Point(center.x,center.y);
    std::cout << "center" << center << std::endl;
    int radius = cvRound(circles[i][2]);
    // circle center
    cv::circle(img_copy, center, 1, cv::Scalar(0,255,0),10);
    // circle outline
    cv::circle(img_copy, center, radius,cv::Scalar(0,255,0),30);

  }

  std::cout << "Pattern center:" << pattern_center << std::endl;
  std::cout << "Num rows:" << gray.rows << std::endl;
  std::cout << "Num cols:" << gray.cols << std::endl;
  std::cout << "Pixels distance:" << gray.rows - pattern_center.y << std::endl;
  int ref_pixels = gray.rows - pattern_center.y;
  int y = gray.rows -(gray.rows - pattern_center.y);
  int x = pattern_center.x;
  cv::Point org(x,y);
  cv::Point end_p(x,gray.rows);
  cv::line(img_copy,org,end_p,cv::Scalar(0,255,0),30);
  std::string out = "Pix ref:";
  std::string ref = std::to_string(ref_pixels);
  out += ref;
  float ref_real = 64.0;
  scale_factor = float(ref_real/float(ref_pixels));
  std::cout << "Real ref:" << ref_real << std::endl;
  std::cout << "ref pixels:" << ref_pixels << std::endl;
  std::cout << "scale factor:" << scale_factor << std::endl;

  cv::putText(img_copy,out ,end_p/2,1,10,cv::Scalar(0,255,0),10);

  cv::namedWindow("pattern",CV_WINDOW_NORMAL);
  cv::resizeWindow("pattern",640,480);
  cv::moveWindow("pattern",0,0);
  cv::imshow("pattern",img_copy);
  cv::waitKey(0);

}

bool Utilities::loadSFM_XML_Data(pcl::PointCloud<pcl::PointXYZ>::Ptr& cloudPCL,bool show){

  std::cout << "Reading sfm_data.xml file. Please wait." << std::endl;

  // Empty document
  tinyxml2::XMLDocument xml_doc;

  // Load xml document
  tinyxml2::XMLError eResult = xml_doc.LoadFile("sfm_data.xml");
  if(eResult != tinyxml2::XML_SUCCESS){
    std::cout << "Error: Could not find a xml file." << std::endl;
    return false;
  }

  std::cout << "Found:" << "sfm_data.xml" << std::endl;

  // Point3D structure for saved pts3d xml document
  std::vector<Point3DInMap> pts3d;
  std::vector<cv::Mat_<float>> cameras_poses;

  // Root xml document
  tinyxml2::XMLNode * root = xml_doc.FirstChildElement("cereal");
  if(root!=nullptr){

    std::cout << "Root tag <cereal>" << std::endl;

    /*INTRINSICS DATA*/
    // Root intrinsics xml document
    tinyxml2::XMLElement * intrinsics = root->FirstChildElement("intrinsics");
    if(intrinsics == nullptr){
      std::cout << "Error: <intrinsics> tag was not found in xml document." << std::endl;
    }

    tinyxml2::XMLElement * data = intrinsics->FirstChildElement("value0")->FirstChildElement("value")->FirstChildElement("ptr_wrapper")->FirstChildElement("data");
    if(data == nullptr){
      std::cout << "Error: <data> tag was not found in xml document." << std::endl;
    }

    float f,cx,cy;
    data->FirstChildElement("focal_length")->QueryFloatText(&f);

    tinyxml2::XMLElement * pp = data->FirstChildElement("principal_point");
    if(pp == nullptr){
      std::cout << "Error: <principal_point> tag was not found in xml document." << std::endl;
    }

    // Iterate over <principal_point> tag
    for(tinyxml2::XMLElement* child = pp->FirstChildElement();child != NULL;
        child = child->NextSiblingElement()){

      cx=cy;
      child->QueryFloatText(&cy);
    }

    std::cout << "f:" << f << " cx:" << cx << " cy:" << cy << std::endl;

    /*EXTRINSICS DATA*/
    // Root extrinsics xml document
    tinyxml2::XMLElement * extrinsics = root->FirstChildElement("extrinsics");
    if(extrinsics == nullptr){
      std::cout << "Error: <extrinsics> tag was not found in xml document." << std::endl;
    }

    int id_camera;


    // Iterate over <extrinsics> tag
    for(tinyxml2::XMLElement* child = extrinsics->FirstChildElement();child != NULL;
        child = child->NextSiblingElement()){

      child->FirstChildElement("key")->QueryIntText(&id_camera);

      tinyxml2::XMLElement * rotation = child->FirstChildElement("value")->FirstChildElement("rotation");
      if(rotation == nullptr){
        std::cout << "Error: <rotation> tag was not found in xml document." << std::endl;
      }

      float r11,r12,r13;
      float r21,r22,r23;
      float r31,r32,r33;

      for(tinyxml2::XMLElement* child = rotation->FirstChildElement();child != NULL;
          child = child->NextSiblingElement()){

        for(tinyxml2::XMLElement* child2 = child->FirstChildElement();child2 != NULL;
            child2 = child2->NextSiblingElement()){

          r11 = r12;
          r12 = r13;
          r13 = r21;
          r21 = r22;
          r22 = r23;
          r23 = r31;
          r31 = r32;
          r32 = r33;
          child2->QueryFloatText(&r33);

        }
      }

      //std::cout << "r11:" << r11 << " r12:" << r12 << " r13:" << r13 << std::endl;
      //std::cout << "r21:" << r21 << " r22:" << r22 << " r23:" << r23 << std::endl;
      //std::cout << "r31:" << r31 << " r32:" << r32 << " r33:" << r33 << std::endl;

      float t1,t2,t3;

      tinyxml2::XMLElement * traslation = child->FirstChildElement("value")->FirstChildElement("center");
      if(traslation == nullptr){
        std::cout << "Error: <center> tag was not found in xml document." << std::endl;
      }

      for(tinyxml2::XMLElement* child = traslation->FirstChildElement();child != NULL;
          child = child->NextSiblingElement()){

        t1 = t2;
        t2 = t3;
        child->QueryFloatText(&t3);
      }

      //std::cout << "t1:" << t1 << " t2:" << t2 << " t3:" << t3 << std::endl;
      cv::Mat_<float> pose = (cv::Mat_<float>(3, 4) << r11, r12, r13, t1,
                                                       r21, r22, r23, t2,
                                                       r31, r32, r33, t3);
      cameras_poses.push_back(pose);

      //break;
    }

    std::cout << "camera:\n" << cameras_poses.at(0) << std::endl;
    std::cout << "Camera poses:" << cameras_poses.size() << " cameras." << std::endl;

    /*POINTCLOUD DATA*/
    // Root structure xml document
    tinyxml2::XMLElement * structure = root->FirstChildElement("structure");
    if(structure == nullptr){
      std::cout << "Error: <structure> tag was not found in xml document." << std::endl;
    }

    // Iterate over <structure> tag
    for(tinyxml2::XMLElement* child = structure->FirstChildElement();child != NULL;
        child = child->NextSiblingElement()){

      Point3DInMap * pt3d = new Point3DInMap();

      tinyxml2::XMLElement * X_root = child->FirstChildElement("value")->FirstChildElement("X");
      if(X_root==nullptr){
        std::cout << "Error: 3D point not found in <X> tag." << std::endl;
        continue;
      }

      float x,y,z;

      // Iterate over <X> tag
      for(tinyxml2::XMLElement* child = X_root->FirstChildElement();child != NULL;
          child = child->NextSiblingElement()){

        x=y;
        y=z;
        eResult = child->QueryFloatText(&z);   //z=coordinate;
      }

      pt3d->pt = cv::Point3f(x,y,z);

      tinyxml2::XMLElement * Observ_root = child->FirstChildElement("value")->FirstChildElement("observations");
      if(Observ_root==nullptr){
        std::cout << "Error: 3D-2d view point not found in <observations> tag." << std::endl;
        continue;
      }

      // Iterate over <observations> tag
      for(tinyxml2::XMLElement* child = Observ_root->FirstChildElement();child != NULL;
          child = child->NextSiblingElement()){

        int img_id;
        child->FirstChildElement("key")->QueryIntText(&img_id);

        tinyxml2::XMLElement * id_feat_root = child->FirstChildElement("value");
        if(id_feat_root==nullptr){
          std::cout << "Error: <id_feat> tag not found." << std::endl;
          continue;
        }

        int img_id_feat;
        id_feat_root->FirstChildElement("id_feat")->QueryIntText(&img_id_feat);

        tinyxml2::XMLElement * feature_root = id_feat_root->FirstChildElement("x");
        if(feature_root==nullptr){
          std::cout << "Error: <x> tag not found." << std::endl;
          continue;
        }

        float x,y;

        // Iterate over <x> tag
        for(tinyxml2::XMLElement* child = feature_root->FirstChildElement();child != NULL;
            child = child->NextSiblingElement()){

          x = y;
          eResult = child->QueryFloatText(&y);   //y=coordinate;
        }

         cv::Point2f pt2d(x,y);
         std::map<const int,cv::Point2f> feature;
         feature[img_id_feat]=pt2d;

         pt3d->feat_ref[img_id]=feature;

      }
      pts3d.push_back(*pt3d);

    }

    /*
    std::cout << "pt3d ini: " << pts3d[0].pt << std::endl;
    std::cout << "pt3d ini num observations:" << pts3d[0].feat_ref.size() << std::endl;
    for(std::pair<const int,std::map<const int,cv::Point2f>>&  ft : pts3d[0].feat_ref){

        for(std::pair<const int,cv::Point2f>&  pt2d : ft.second){
          std::cout << "img:" << ft.first <<" ft_id:" << pt2d.first << " pt2d:" << pt2d.second << std::endl;
        }
    }

    std::cout << "pt3d end: " << pts3d[pts3d.size()-1].pt << std::endl;
    std::cout << "pt3d end num observations:" << pts3d[pts3d.size()-1].feat_ref.size() << std::endl;
    for(std::pair<const int,std::map<const int,cv::Point2f>>&  ft : pts3d[pts3d.size()-1].feat_ref){

        for(std::pair<const int,cv::Point2f>&  pt2d : ft.second){
          std::cout << "img:" << ft.first <<" ft_id:" << pt2d.first << " pt2d:" << pt2d.second << std::endl;
        }
    }
    */
    std::cout << "pointcloud size:" << pts3d.size() << std::endl;

    fromPoint3DToPCLCloud(pts3d,cloudPCL);

  }else{
    std::cout << "Error: root of xml could not found. Must be: <cereal>" << std::endl;
    return false;
  }

  if(show){

    pcl::visualization::PCLVisualizer viewer = pcl::visualization::PCLVisualizer("XML pointcloud",true);

    viewer.setPosition(0,0);
    viewer.setSize(640,480);
    viewer.setBackgroundColor(0.05, 0.05, 0.05, 0);
    viewer.addCoordinateSystem();
    viewer.setCameraPosition(0,0,1,0,0,0);
    pcl::PointXYZ p1, p2, p3;
    p1.getArray3fMap() << 1, 0, 0;
    p2.getArray3fMap() << 0, 1, 0;
    p3.getArray3fMap() << 0,0.1,1;

    viewer.addText3D("x", p1, 0.2, 1, 0, 0, "x_");
    viewer.addText3D("y", p2, 0.2, 0, 1, 0, "y_");
    viewer.addText3D ("z", p3, 0.2, 0, 0, 1, "z_");
    viewer.addPointCloud(cloudPCL,"xml_pointcloud");
    viewer.resetCamera();

    std::cout << "Press [q] to continue!" << std::endl;

    while(!viewer.wasStopped ()) {
           viewer.spin();
    }
  }

  return true;
}

void Utilities::fromPoint3DToPCLCloud(const std::vector<Point3DInMap> &input_cloud,
                           pcl::PointCloud<pcl::PointXYZ>::Ptr& cloudPCL){

  for(size_t i = 0; i < input_cloud.size(); ++i){
      cv::Point3f pt3d = input_cloud[i].pt;
      pcl::PointXYZ pclp;
      pclp.x  = pt3d.x;
      pclp.y  = pt3d.y;
      pclp.z  = pt3d.z;
      cloudPCL->push_back(pclp);
   }
   cloudPCL->width = (uint32_t) cloudPCL->points.size(); // number of points
   cloudPCL->height = 1;	// a list, one row of data
   cloudPCL->header.frame_id ="map";
   cloudPCL->is_dense = false;
}




