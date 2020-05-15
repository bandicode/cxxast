
int bar();
int qux();

int foo()
{
  return 4;
}

int bar()
{
  return foo() + 6;
}
