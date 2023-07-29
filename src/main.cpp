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

    if (rr < 0 || rr >= maze.rows || cc < 0 || cc >= maze.cols || maze.at<uchar>(rr, cc) == 0)
      continue;

    maze.at<uchar>(r + dr[d[i]], c + dc[d[i]]) = 0;
    maze.at<uchar>(rr, cc) = 0;

    carve(maze, rr, cc);
  }
}

cv::Mat generate_maze(int rows, int cols, int pathSize) {
  int actualRows = 2 * (rows / (2 * pathSize)) + 1;
  int actualCols = 2 * (cols / (2 * pathSize)) + 1;

  cv::Mat maze(actualRows, actualCols, CV_8UC1, cv::Scalar(255));

  std::random_device rd;
  std::mt19937 generator(rd());
  int startRow = std::uniform_int_distribution<int>{0, actualRows / 2}(generator) * 2 + 1;
  int startCol = std::uniform_int_distribution<int>{0, actualCols / 2}(generator) * 2 + 1;
  maze.at<uchar>(startRow, startCol) = 0;

  carve(maze, startRow, startCol);

  cv::Mat largeMaze;
  cv::resize(maze, largeMaze, cv::Size(cols, rows), 0, 0, cv::INTER_NEAREST);
  return largeMaze;
}

int main() {
  int mazeWidth = 1000;
  int mazeHeight = 1000;
  int pathSize = 30;

  cv::Mat maze = generate_maze(mazeHeight, mazeWidth, pathSize);
  cv::imshow("Maze", maze);
  cv::waitKey(0);

  return 0;
}
