
// Should be parsed with CXTranslationUnit_DetailedPreprocessingRecord 

/* hello */
// world

int main()
{
  #define FOO
  
  #ifdef FOO
	return 0;
  #else
    return 1;
  #endif
}
