#pragma once
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

cv::Mat cropArea(const cv::Mat& maze, int x, int y, int width, int height) {
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

cv::Point templateMatch(const cv::Mat& image, const cv::Mat& target) {
  int overlap_x = std::min(image.cols, target.cols);
  int overlap_y = std::min(image.rows, target.rows);

  cv::Mat overlapTarget = target(cv::Rect(0, 0, overlap_x, overlap_y));

  int result_cols = image.cols - overlapTarget.cols + 1;
  int result_rows = image.rows - overlapTarget.rows + 1;

  cv::Mat result;
  result.create(result_rows, result_cols, CV_32FC1);

  matchTemplate(image, overlapTarget, result, cv::TM_CCOEFF_NORMED);
  normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  cv::Point matchLoc;

  minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

  matchLoc = maxLoc;

  return matchLoc;
}

// depends on MOVE_BY const
int MOVE_BY_CROP = 20;

struct StitchResult {
  cv::Mat stitched;
  cv::Point matchLoc;
};

StitchResult stitch(const cv::Mat& mapped, const cv::Mat& next) {
  // template matching doesn't work well if the match is not fully inside
  auto nextSmaller = cropArea(
      next,
      MOVE_BY_CROP,
      MOVE_BY_CROP,
      next.cols - MOVE_BY_CROP * 2,
      next.rows - MOVE_BY_CROP * 2
  );
  cv::Point matchLoc = templateMatch(mapped, nextSmaller);

  cv::Point normalizeMatchLoc = {
      std::max(0, matchLoc.x - 20),
      std::max(0, matchLoc.y - 20)};

  int stitchedCols = std::max(mapped.cols, normalizeMatchLoc.x + next.cols);
  int stitchedRows = std::max(mapped.rows, normalizeMatchLoc.y + next.rows);

  cv::Mat stitched(stitchedRows, stitchedCols, CV_8UC1, cv::Scalar(255));

  mapped.copyTo(stitched(cv::Rect(0, 0, mapped.cols, mapped.rows)));
  next.copyTo(stitched(
      cv::Rect(normalizeMatchLoc.x, normalizeMatchLoc.y, next.cols, next.rows)
  ));

  return {stitched, normalizeMatchLoc};
}
