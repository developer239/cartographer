#include <opencv2/opencv.hpp>
#include <random>

void carve(cv::Mat& maze, int r, int c) {
  static const int dr[] = {-1, 1, 0, 0};
  static const int dc[] = {0, 0, -1, 1};

  std::random_device rd;
  std::mt19937 generator(rd());
  std::vector<int> d{0, 1, 2, 3};
  std::shuffle(d.begin(), d.end(), generator);

  for (int i = 0; i < 4; ++i) {
    int rr = r + 2 * dr[d[i]];
    int cc = c + 2 * dc[d[i]];

    if (rr < 0 || rr >= maze.rows || cc < 0 || cc >= maze.cols ||
        maze.at<uchar>(rr, cc) == 0)
      continue;

    maze.at<uchar>(r + dr[d[i]], c + dc[d[i]]) = 0;
    maze.at<uchar>(rr, cc) = 0;

    carve(maze, rr, cc);
  }
}

cv::Mat generateMaze(int rows, int cols, int pathSize) {
  int actualRows = 2 * (rows / (2 * pathSize)) + 1;
  int actualCols = 2 * (cols / (2 * pathSize)) + 1;

  cv::Mat maze(actualRows, actualCols, CV_8UC1, cv::Scalar(255));

  std::random_device rd;
  std::mt19937 generator(rd());
  int startRow =
      std::uniform_int_distribution<int>{0, actualRows / 2}(generator)*2 + 1;
  int startCol =
      std::uniform_int_distribution<int>{0, actualCols / 2}(generator)*2 + 1;
  maze.at<uchar>(startRow, startCol) = 0;

  carve(maze, startRow, startCol);

  cv::Mat largeMaze;
  cv::resize(maze, largeMaze, cv::Size(cols, rows), 0, 0, cv::INTER_NEAREST);
  return largeMaze;
}

cv::Mat cropArea(cv::Mat& maze, int x, int y, int width, int height) {
  cv::Rect roi(x, y, width, height);
  return maze(roi);
}

cv::Mat cropRandomArea(cv::Mat& maze, int width, int height) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<int> xDist{0, maze.cols - width};
  std::uniform_int_distribution<int> yDist{0, maze.rows - height};

  int x = xDist(generator);
  int y = yDist(generator);

  return cropArea(maze, x, y, width, height);
}

cv::Point templateMatch(cv::Mat image, cv::Mat target) {
  int result_cols = image.cols - target.cols + 1;
  int result_rows = image.rows - target.rows + 1;

  cv::Mat result;
  result.create(result_rows, result_cols, CV_32FC1);

  matchTemplate(image, target, result, cv::TM_CCOEFF_NORMED);
  normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  cv::Point matchLoc;

  minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

  matchLoc = maxLoc;

  rectangle(
      image,
      matchLoc,
      cv::Point(matchLoc.x + target.cols, matchLoc.y + target.rows),
      cv::Scalar::all(0),
      2,
      8,
      0
  );
  rectangle(
      result,
      matchLoc,
      cv::Point(matchLoc.x + target.cols, matchLoc.y + target.rows),
      cv::Scalar::all(0),
      2,
      8,
      0
  );

  return matchLoc;
}

int main() {
  int mazeWidth = 1000;
  int mazeHeight = 1000;
  int pathSize = 30;

  cv::Mat map = generateMaze(mazeHeight, mazeWidth, pathSize);
  cv::Mat minimap = cropRandomArea(map, 100, 100);

  auto location = templateMatch(map, minimap);
  cv::rectangle(
      map,
      location,
      cv::Point(location.x + minimap.cols, location.y + minimap.rows),
      cv::Scalar::all(0),
      2,
      8,
      0
  );

  imshow("map", map);
  imshow("minimap", minimap);

  cv::waitKey(0);

  return 0;
}
