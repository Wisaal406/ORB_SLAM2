/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<thread>
#include<opencv2/core/core.hpp>
#include <vector>
#include <map>
#include<System.h>

#include <boost/filesystem.hpp>
#include <vector>
#include <cinttypes>
#include <stdexcept>
#include <memory>

namespace fs = boost::filesystem;

using namespace std;
using boost::property_tree::ptree;
void show_interesting_object(std::map<long unsigned int, std::vector<ORB_SLAM2::TrafficSign> > &image_trafficsigns_map);
bool ExtractSemanticObjGrp(std::string jsonFilename, std::map<long unsigned int, std::vector<ORB_SLAM2::TrafficSign> > &SemanticObjGrp);
void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);

int gImgWidth = 1280;
int gImgHeight = 720;
int gMinRectWidth = 90;
int gMinRectHeight = 90;

struct input_args {
    std::string path_to_vocabulary;
    std::string path_to_camera_settings;
    std::string path_to_image_folder;
    std::string path_to_json_file;
};

input_args parse_input_arguments(int argc, char** argv)
{
    if (argc < 4) {
        throw std::runtime_error(
            "Usage: ./semantic_monocular path_to_vocabulary path_to_camera_settings path_to_image_folder path_to_jsonfile");
    }

    return input_args{
        argv[1],
        argv[2],
        argv[3],
        argv[4],
    };
}

class slam_object {
    ORB_SLAM2::System _slam;

    void shutdown()
    {
        // Stop all threads
        _slam.Shutdown();

        // Save camera trajectory
        _slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
    }
public:
    ORB_SLAM2::System& get()
    {
        return _slam;
    }

    ORB_SLAM2::System const& get() const
    {
        return _slam;
    }

    slam_object(input_args const& args_)
        : _slam(
            args_.path_to_vocabulary,
            args_.path_to_camera_settings,
            ORB_SLAM2::System::MONOCULAR,
            true)
    {}

    ~slam_object()
    {
        shutdown();
    }
};
void Pause(int WaitTime)
{
    //pause on entering space bar
    if (cv::waitKey(WaitTime) == 32)
        cv::waitKey();
}

int run_slam_loop(int argc, char** argv)
{
    try {
        auto args = parse_input_arguments(argc, argv);

        std::vector<fs::path> image_files;
        std::copy(fs::directory_iterator(args.path_to_image_folder), fs::directory_iterator(),
                  std::back_inserter(image_files));
        std::sort(image_files.begin(), image_files.end());

        ORB_SLAM2::traffic_sign_map_t traffic_signs;
        // Create SLAM system. It initializes all system threads and gets ready to process frames.

        slam_object slam{args};
        if (ExtractSemanticObjGrp(args.path_to_json_file, traffic_signs)) {
            slam.get().SetSemanticObjGrpContent(traffic_signs);
        }

        std::uint64_t time = 0;
        for (auto const& file : image_files) {
            auto image = cv::imread(file.generic_string(), CV_LOAD_IMAGE_UNCHANGED);

            if (image.empty()) {
                throw std::runtime_error("Failed to load image!");
            }

            // Pass the image to the SLAM system
            slam.get().TrackMonocular(image, static_cast<double>(time));
            Pause(70);
            time++;
        }
    } catch (std::exception const& ex_) {

        std::cerr << "Exception: " << ex_.what() << std::endl;

        return 1;
    }

    return 0;
}

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream f;
    f.open(strFile.c_str());

    // skip first three lines
    string s0;

    while (!f.eof()) {
        string s;
        getline(f, s);
        if (!s.empty()) {
            stringstream ss;
            ss << s;
            double t;
            string sRGB;
            ss >> t;
            vTimestamps.push_back(t);
            ss >> sRGB;
            vstrImageFilenames.push_back(sRGB);
        }
    }
}
void TransformRect(std::vector<double> &RectArr, cv::Rect& Roi, bool IsAbsolute = false)
{

    if (true == IsAbsolute) {
        Roi.x = RectArr[0];
        Roi.y = RectArr[1];
        Roi.width = RectArr[2] - RectArr[0];
        Roi.height = RectArr[3] - RectArr[1];
    } else {
        int ymin = int(RectArr[0] * gImgHeight);
        int xmin = int(RectArr[1] * gImgWidth);
        int ymax = int(RectArr[2] * gImgHeight);
        int xmax = int(RectArr[3] * gImgWidth);
        Roi.x = xmin;
        Roi.y = ymin;
        Roi.width = xmax - xmin;
        Roi.height = ymax - ymin;
    }
}

void enlarge_rectangle(cv::Rect& rectangle)
{
    if (rectangle.width >= gMinRectWidth && rectangle.height >= gMinRectHeight) {
        return;
    }
    if (rectangle.width < gMinRectWidth) {
        int total_x_displacement = gMinRectWidth - rectangle.width;
        int x_displacement = floor(total_x_displacement / 2);

        if (((rectangle.x - x_displacement) >= 0) && ((rectangle.x + rectangle.width + x_displacement) < gImgWidth)) {
            rectangle.x -= x_displacement;
            rectangle.width = gMinRectWidth;
        } else if ((rectangle.x + gMinRectWidth) <= gImgWidth)
            rectangle.width = gMinRectWidth;
    }
    if (rectangle.height < gMinRectHeight) {
        int total_y_displacement = gMinRectHeight - rectangle.height;
        int y_displacement = floor(total_y_displacement / 2);
        if (((rectangle.y - y_displacement) >= 0) && ((rectangle.y + rectangle.height + y_displacement) < gImgHeight)) {
            rectangle.y -= y_displacement;
            rectangle.height = gMinRectHeight;
        } else if ((rectangle.y + gMinRectHeight) <= gImgHeight)
            rectangle.height = gMinRectHeight;
    }
}
bool ExtractSemanticObjGrp(std::string jsonFilename, std::map<long unsigned int, std::vector<ORB_SLAM2::TrafficSign> > &SemanticObjGrp)
{
    boost::property_tree::ptree pt;
    std::fstream jsonfile(jsonFilename);
    if (false == jsonfile.is_open()) {
        cout << "Unable to open json file" << endl;
        return false;
    }

    boost::property_tree::read_json(jsonfile, pt);
    jsonfile.close();


    for (ptree::iterator pt_iter = pt.begin(); pt_iter != pt.end(); pt_iter++) {
        std::string image_name = pt_iter->first;
        auto &traffic_sign_arr = pt_iter->second;
        std::vector<ORB_SLAM2::TrafficSign> traffic_signs;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &node, traffic_sign_arr.get_child("traffic_signs"))
        {
            ORB_SLAM2::TrafficSign t;
            t.classid = node.second.get<int>("class_id");
            t.confidence = node.second.get<double>("confidence");
            std::vector<double> r;
            for (auto &temppt : node.second.get_child("rectangle")) {
                r.push_back(temppt.second.get_value < double >());
            }
            TransformRect(r, t.Roi);

            //enlarge_rectangle(t.Roi);

            traffic_signs.push_back(t);

        }

        SemanticObjGrp.insert({stoul(image_name), traffic_signs});
    }

    return true;
}

void show_interesting_object(std::map<long unsigned int, std::vector<ORB_SLAM2::TrafficSign> > &image_trafficsigns_map)
{

    for (auto &map_item : image_trafficsigns_map) {

        std::cout << "image- " << map_item.first << ":" << std::endl;
        for (auto &vector_item : map_item.second) {
            std::cout << "\ttraffic_signs: " << std::endl;
            std::cout << "\t\tclass_id- " << vector_item.classid << std::endl;
            std::cout << "\t\tconfidence- " << std::setprecision(16) << vector_item.confidence << std::endl;
            std::cout << "\t\trectangle- [";

            std::cout << vector_item.Roi.x << "  ";
            std::cout << vector_item.Roi.y << "  ";
            std::cout << vector_item.Roi.width << "  ";
            std::cout << vector_item.Roi.height;
            std::cout << "]" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    return run_slam_loop(argc, argv);
}
