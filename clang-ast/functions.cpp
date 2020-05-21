
void foo(int a = 3 + 5)
{
  int b = a + 2;
}

int add(int a, int b)
{
  return a + b;
}

int bar() = delete;

struct A { };

A aa();
