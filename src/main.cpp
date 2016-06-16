#include <pcl/console/print.h>
#include <pcl/console/parse.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <boost/smart_ptr/make_shared.hpp>

// Typedefs
//using PointType = pcl::PointXYZRGBA;
//using Cloud = pcl::PointCloud <PointType>;

namespace fs = boost::filesystem;

static constexpr auto INVALID_CLOUD = -1;
static constexpr auto UNORGANISED = 0;
static constexpr auto ORGANISED = 1;
static constexpr auto ERROR_OUTPUT = "The file specified was not a valid point cloud.";
static constexpr auto ORGANISED_OUTPUT = "Organised";
static constexpr auto UNORGANISED_OUTPUT = "Not organised";


auto printHelp (int argc, char ** argv)
-> void {
  using pcl::console::print_error;
  using pcl::console::print_info;

  // TODO: Update this help
  print_error ("Syntax is: %s <path-to-pcd-file> <options> | -h | --help\n", argv[0]);
  print_info ("%s -h | --help : shows this help\n", argv[0]);
  print_info ("-t T : the type of cloud to be parsed (default: xyzrgba), must be one of :\n");

  // TODO: Add the other possible types
  print_info ("       - xyzrgba\n");
  print_info ("       - xyz\n");
}


auto expandTilde (std::string path_string) -> fs::path {
  if (path_string.at (0) == '~')
    path_string.replace (0, 1, getenv ("HOME"));
  return fs::path{path_string};
}

template <typename PointType>
auto isOrganised (typename pcl::PointCloud<PointType>::Ptr const & cloud) {
  if (!cloud) {
    return INVALID_CLOUD;
  } else if (cloud->isOrganized ()) {
    return ORGANISED;
  } else {
    return UNORGANISED;
  }
}

template <typename PointType>
auto loadCloud (fs::path const & filepath)
-> typename pcl::PointCloud <PointType>::Ptr {
  auto const cloud = boost::make_shared <pcl::PointCloud <PointType>> ();
  if (pcl::io::loadPCDFile <PointType> (filepath.c_str (), *cloud) != -1)
    return cloud;
  else
    return nullptr;
}

auto outputResults (int result_code) {
  switch (result_code) {
    case INVALID_CLOUD:
      std::cout << ERROR_OUTPUT;
      break;
    case ORGANISED:
      std::cout << ORGANISED_OUTPUT;
      break;
    case UNORGANISED:
      std::cout << UNORGANISED_OUTPUT;
      break;
  }
  std::cout << std::endl;
}

auto main(int argc, char ** argv)
-> int {
  auto const pcd_arg_index = pcl::console::parse_file_extension_argument (argc, argv, ".pcd");

  if(pcd_arg_index.size () != 1) {
    std::cout << "Please ensure one pcd file is specified." << std::endl;
    printHelp (argc, argv);
    return -1;
  }

  auto const input_pcd_file = expandTilde (std::string {argv[pcd_arg_index.at (0)]});

  // TODO: Parse arg for different cloud types here
  auto cloud = loadCloud <pcl::PointXYZRGBA> (input_pcd_file);
  auto result = isOrganised <pcl::PointXYZRGBA> (cloud);
  outputResults (result);

  return 0;
}