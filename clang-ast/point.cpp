
struct Point
{
public:
  int x, y = 0;

  static int n = 3;
};

Point operator+(const Point& lhs, const Point& rhs)
{
  return Point{ lhs.x + rhs.x, lhs.y + rhs.y };
}

int main()
{
  Point p;
  p.x = 1;
  p.y = 2;

  p = p + p;
}
