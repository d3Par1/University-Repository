#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>

using namespace std;

// Structure to represent a point on the grid
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

// Structure to represent a path between two points
struct Path {
    vector<Point> points;
    bool isValid(const vector<vector<bool>>& used) const {
        // Check if path uses already used cells
        for (const Point& p : points) {
            if (used[p.x][p.y]) return false;
        }
        
        // Check 2x2 constraint
        for (size_t i = 0; i < points.size() - 1; i++) {
            if (violates2x2Rule(points[i], points[i + 1])) {
                return false;
            }
        }
        return true;
    }
    
    bool violates2x2Rule(const Point& p1, const Point& p2) const {
        // Check if path segment uses all cells in any 2x2 area
        if (abs(p1.x - p2.x) + abs(p1.y - p2.y) > 3) {
            return true;
        }
        return false;
    }
};

class PuzzleSolver {
private:
    int size;
    vector<vector<char>> grid;
    vector<vector<bool>> used;
    map<char, vector<Point>> letterPositions;
    vector<Path> solution;

public:
    PuzzleSolver(const vector<vector<char>>& input) : 
        size(input.size()), 
        grid(input),
        used(size, vector<bool>(size, false)) {
        // Find positions of all letters
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (isalpha(grid[i][j])) {
                    letterPositions[grid[i][j]].push_back(Point(i, j));
                }
            }
        }
    }

    // Find path between two points using BFS
    Path findPath(const Point& start, const Point& end) {
        queue<vector<Point>> q;
        set<Point> visited;
        q.push({start});
        visited.insert(start);

        while (!q.empty()) {
            vector<Point> currentPath = q.front();
            q.pop();
            Point current = currentPath.back();

            if (current == end) {
                Path path;
                path.points = currentPath;
                return path;
            }

            // Try all four directions
            const int dx[] = {-1, 1, 0, 0};
            const int dy[] = {0, 0, -1, 1};
            
            for (int i = 0; i < 4; i++) {
                Point next(current.x + dx[i], current.y + dy[i]);
                
                if (next.x >= 0 && next.x < size && 
                    next.y >= 0 && next.y < size && 
                    visited.find(next) == visited.end()) {
                    
                    vector<Point> newPath = currentPath;
                    newPath.push_back(next);
                    
                    Path tempPath;
                    tempPath.points = newPath;
                    
                    if (tempPath.isValid(used)) {
                        visited.insert(next);
                        q.push(newPath);
                    }
                }
            }
        }
        return Path(); // Empty path if no solution found
    }

    bool solve() {
        // Try to connect each pair of same letters
        for (const auto& pair : letterPositions) {
            if (pair.second.size() != 2) continue; // Skip if not exactly 2 positions
            
            Path path = findPath(pair.second[0], pair.second[1]);
            if (path.points.empty()) return false;
            
            // Mark path as used
            for (const Point& p : path.points) {
                used[p.x][p.y] = true;
            }
            solution.push_back(path);
        }
        return true;
    }

    void printSolution() {
        vector<vector<char>> result = grid;
        
        // Mark paths
        for (const Path& path : solution) {
          for (size_t i = 0; i < path.points.size() - 1; i++) {
            Point current = path.points[i];
            Point next = path.points[i + 1];
            
            if (current.x == next.x) {
                result[current.x][current.y] = '-';
            } else {
                result[current.x][current.y] = '|';
            }
        }
    }

    // Print result
    for (const auto& row : result) {
        for (char c : row) {
            cout << c << ' ';
        }
        cout << endl;
    }
}
};

int main() {
// Example puzzle from the image
vector<vector<char>> puzzle = {
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'S', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'S', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ' '},
    {' ', ' ', ' ', 'C', ' ', ' ', ' ', ' ', 'L', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', 'O', ' ', ' ', ' ', ' ', ' ', ' ', 'N', ' '},
    {' ', ' ', ' ', ' ', ' ', 'L', ' ', ' ', 'C', ' ', ' ', ' ', ' '},
    {' ', 'G', ' ', ' ', ' ', ' ', 'O', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'G', ' ', ' ', ' ', ' ', ' '},
    {'Z', ' ', ' ', 'E', ' ', ' ', ' ', ' ', 'N', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', 'T', ' ', ' '},
    {' ', ' ', ' ', 'T', ' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', 'Z', ' ', ' ', ' ', ' ', ' ', 'M'},
    {' ', ' ', ' ', ' ', ' ', ' ', 'M', ' ', ' ', ' ', ' ', ' ', ' '}
};

PuzzleSolver solver(puzzle);
if (solver.solve()) {
    solver.printSolution();
} else {
    cout << "No solution found!" << endl;
}

return 0;
}