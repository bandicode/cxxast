// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/libclang-parser.h"

namespace cxx
{

namespace parsers
{

static CXVersion parse_clang_version(std::string str)
{
  CXVersion result;

  auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

  auto it = str.begin();

  while (!is_digit(*it)) ++it;

  auto major_begin = it;

  while (is_digit(*it)) ++it;

  result.Major = std::stoi(std::string(major_begin, it));

  while (!is_digit(*it)) ++it;

  auto minor_begin = it;

  while (is_digit(*it)) ++it;

  result.Minor = std::stoi(std::string(minor_begin, it));

  while (!is_digit(*it)) ++it;

  auto subminor_begin = it;

  while (it != str.end() && is_digit(*it)) ++it;

  result.Subminor = std::stoi(std::string(subminor_begin, it));


  return result;
}

LibClangParser::LibClangParser()
{
  libclang.reset(new dynlib::Library("libclang"));

  if (!libclang->load())
    throw LibClangParserError{ "could not load libclang" };

  resolve("clang_getClangVersion", clang_getClangVersion);
  resolve("clang_getCString", clang_getCString);
  resolve("clang_disposeString", clang_disposeString);

  m_printable_version = toStdString(clang_getClangVersion());
  m_version = parse_clang_version(m_printable_version);

  resolve("clang_disposeStringSet", clang_disposeStringSet);

  resolve("clang_createIndex", clang_createIndex);
  resolve("clang_disposeIndex", clang_disposeIndex);
  resolve("clang_CXIndex_setGlobalOptions", clang_CXIndex_setGlobalOptions);
  resolve("clang_CXIndex_getGlobalOptions", clang_CXIndex_getGlobalOptions);
  resolve("clang_CXIndex_setInvocationEmissionPathOption", clang_CXIndex_setInvocationEmissionPathOption);

  resolve("clang_getFileName", clang_getFileName);
  resolve("clang_getFileUniqueID", clang_getFileUniqueID);
  resolve("clang_isFileMultipleIncludeGuarded", clang_isFileMultipleIncludeGuarded);
  resolve("clang_getFile", clang_getFile);
  resolve("clang_getFileContents", clang_getFileContents);
  resolve("clang_File_isEqual", clang_File_isEqual);
  resolve("clang_File_tryGetRealPathName", clang_File_tryGetRealPathName);

  resolve("clang_getNullLocation", clang_getNullLocation);
  resolve("clang_equalLocations", clang_equalLocations);
  resolve("clang_getLocation", clang_getLocation);
  resolve("clang_getLocationForOffset", clang_getLocationForOffset);
  resolve("clang_Location_isInSystemHeader", clang_Location_isInSystemHeader);
  resolve("clang_Location_isFromMainFile", clang_Location_isFromMainFile);
  resolve("clang_getNullRange", clang_getNullRange);
  resolve("clang_getRange", clang_getRange);
  resolve("clang_equalRanges", clang_equalRanges);
  resolve("clang_Range_isNull", clang_Range_isNull);
  resolve("clang_getExpansionLocation", clang_getExpansionLocation);
  resolve("clang_getPresumedLocation", clang_getPresumedLocation);
  resolve("clang_getInstantiationLocation", clang_getInstantiationLocation);
  resolve("clang_getSpellingLocation", clang_getSpellingLocation);
  resolve("clang_getFileLocation", clang_getFileLocation);
  resolve("clang_getRangeStart", clang_getRangeStart);
  resolve("clang_getRangeEnd", clang_getRangeEnd);
  resolve("clang_getSkippedRanges", clang_getSkippedRanges);
  resolve("clang_getAllSkippedRanges", clang_getAllSkippedRanges);
  resolve("clang_disposeSourceRangeList", clang_disposeSourceRangeList);

  resolve("clang_getNumDiagnosticsInSet", clang_getNumDiagnosticsInSet);
  resolve("clang_getDiagnosticInSet", clang_getDiagnosticInSet);
  resolve("clang_loadDiagnostics", clang_loadDiagnostics);
  resolve("clang_disposeDiagnosticSet", clang_disposeDiagnosticSet);
  resolve("clang_getChildDiagnostics", clang_getChildDiagnostics);
  resolve("clang_getNumDiagnostics", clang_getNumDiagnostics);
  resolve("clang_getDiagnostic", clang_getDiagnostic);
  resolve("clang_getDiagnosticSetFromTU", clang_getDiagnosticSetFromTU);
  resolve("clang_disposeDiagnostic", clang_disposeDiagnostic);
  resolve("clang_formatDiagnostic", clang_formatDiagnostic);
  resolve("clang_defaultDiagnosticDisplayOptions", clang_defaultDiagnosticDisplayOptions);
  resolve("clang_getDiagnosticSeverity", clang_getDiagnosticSeverity);
  resolve("clang_getDiagnosticLocation", clang_getDiagnosticLocation);
  resolve("clang_getDiagnosticSpelling", clang_getDiagnosticSpelling);
  resolve("clang_getDiagnosticOption", clang_getDiagnosticOption);
  resolve("clang_getDiagnosticCategory", clang_getDiagnosticCategory);
  resolve("clang_getDiagnosticCategoryText", clang_getDiagnosticCategoryText);
  resolve("clang_getDiagnosticNumRanges", clang_getDiagnosticNumRanges);
  resolve("clang_getDiagnosticRange", clang_getDiagnosticRange);
  resolve("clang_getDiagnosticNumFixIts", clang_getDiagnosticNumFixIts);
  resolve("clang_getDiagnosticFixIt", clang_getDiagnosticFixIt);
  resolve("clang_getTranslationUnitSpelling", clang_getTranslationUnitSpelling);
  resolve("clang_createTranslationUnitFromSourceFile", clang_createTranslationUnitFromSourceFile);
  resolve("clang_createTranslationUnit", clang_createTranslationUnit);
  resolve("clang_createTranslationUnit2", clang_createTranslationUnit2);
  resolve("clang_defaultEditingTranslationUnitOptions", clang_defaultEditingTranslationUnitOptions);
  resolve("clang_parseTranslationUnit", clang_parseTranslationUnit);
  resolve("clang_parseTranslationUnit2", clang_parseTranslationUnit2);
  resolve("clang_parseTranslationUnit2FullArgv", clang_parseTranslationUnit2FullArgv);
  resolve("clang_defaultSaveOptions", clang_defaultSaveOptions);
  resolve("clang_saveTranslationUnit", clang_saveTranslationUnit);
  resolve("clang_suspendTranslationUnit", clang_suspendTranslationUnit);
  resolve("clang_disposeTranslationUnit", clang_disposeTranslationUnit);
  resolve("clang_defaultReparseOptions", clang_defaultReparseOptions);
  resolve("clang_reparseTranslationUnit", clang_reparseTranslationUnit);
  resolve("clang_getTUResourceUsageName", clang_getTUResourceUsageName);
  resolve("clang_getCXTUResourceUsage", clang_getCXTUResourceUsage);
  resolve("clang_disposeCXTUResourceUsage", clang_disposeCXTUResourceUsage);
  resolve("clang_getTranslationUnitTargetInfo", clang_getTranslationUnitTargetInfo);
  resolve("clang_TargetInfo_dispose", clang_TargetInfo_dispose);
  resolve("clang_TargetInfo_getTriple", clang_TargetInfo_getTriple);
  resolve("clang_TargetInfo_getPointerWidth", clang_TargetInfo_getPointerWidth);
  resolve("clang_getNullCursor", clang_getNullCursor);
  resolve("clang_getTranslationUnitCursor", clang_getTranslationUnitCursor);
  resolve("clang_equalCursors", clang_equalCursors);
  resolve("clang_Cursor_isNull", clang_Cursor_isNull);
  resolve("clang_hashCursor", clang_hashCursor);
  resolve("clang_getCursorKind", clang_getCursorKind);
  resolve("clang_isDeclaration", clang_isDeclaration);
  resolve("clang_isInvalidDeclaration", clang_isInvalidDeclaration);
  resolve("clang_isReference", clang_isReference);
  resolve("clang_isExpression", clang_isExpression);
  resolve("clang_isStatement", clang_isStatement);
  resolve("clang_isAttribute", clang_isAttribute);
  resolve("clang_Cursor_hasAttrs", clang_Cursor_hasAttrs);
  resolve("clang_isInvalid", clang_isInvalid);
  resolve("clang_isTranslationUnit", clang_isTranslationUnit);
  resolve("clang_isPreprocessing", clang_isPreprocessing);
  resolve("clang_isUnexposed", clang_isUnexposed);
  resolve("clang_getCursorLinkage", clang_getCursorLinkage);
  resolve("clang_getCursorVisibility", clang_getCursorVisibility);
  resolve("clang_getCursorAvailability", clang_getCursorAvailability);
  resolve("clang_getCursorPlatformAvailability", clang_getCursorPlatformAvailability);
  resolve("clang_disposeCXPlatformAvailability", clang_disposeCXPlatformAvailability);
  resolve("clang_getCursorLanguage", clang_getCursorLanguage);
  resolve("clang_getCursorTLSKind", clang_getCursorTLSKind);
  resolve("clang_Cursor_getTranslationUnit", clang_Cursor_getTranslationUnit);
  resolve("clang_createCXCursorSet", clang_createCXCursorSet);
  resolve("clang_disposeCXCursorSet", clang_disposeCXCursorSet);
  resolve("clang_CXCursorSet_contains", clang_CXCursorSet_contains);
  resolve("clang_CXCursorSet_insert", clang_CXCursorSet_insert);
  resolve("clang_getCursorSemanticParent", clang_getCursorSemanticParent);
  resolve("clang_getCursorLexicalParent", clang_getCursorLexicalParent);
  resolve("clang_getOverriddenCursors", clang_getOverriddenCursors);
  resolve("clang_disposeOverriddenCursors", clang_disposeOverriddenCursors);
  resolve("clang_getIncludedFile", clang_getIncludedFile);
  resolve("clang_getCursor", clang_getCursor);
  resolve("clang_getCursorLocation", clang_getCursorLocation);
  resolve("clang_getCursorExtent", clang_getCursorExtent);
  resolve("clang_getCursorType", clang_getCursorType);
  resolve("clang_getTypeSpelling", clang_getTypeSpelling);
  resolve("clang_getTypedefDeclUnderlyingType", clang_getTypedefDeclUnderlyingType);
  resolve("clang_getEnumDeclIntegerType", clang_getEnumDeclIntegerType);
  resolve("clang_getEnumConstantDeclValue", clang_getEnumConstantDeclValue);
  resolve("clang_getEnumConstantDeclUnsignedValue", clang_getEnumConstantDeclUnsignedValue);
  resolve("clang_getFieldDeclBitWidth", clang_getFieldDeclBitWidth);
  resolve("clang_Cursor_getNumArguments", clang_Cursor_getNumArguments);
  resolve("clang_Cursor_getArgument", clang_Cursor_getArgument);
  resolve("clang_Cursor_getNumTemplateArguments", clang_Cursor_getNumTemplateArguments);
  resolve("clang_Cursor_getTemplateArgumentKind", clang_Cursor_getTemplateArgumentKind);
  resolve("clang_Cursor_getTemplateArgumentType", clang_Cursor_getTemplateArgumentType);
  resolve("clang_Cursor_getTemplateArgumentValue", clang_Cursor_getTemplateArgumentValue);
  resolve("clang_Cursor_getTemplateArgumentUnsignedValue", clang_Cursor_getTemplateArgumentUnsignedValue);
  resolve("clang_equalTypes", clang_equalTypes);
  resolve("clang_getCanonicalType", clang_getCanonicalType);
  resolve("clang_isConstQualifiedType", clang_isConstQualifiedType);
  resolve("clang_Cursor_isMacroFunctionLike", clang_Cursor_isMacroFunctionLike);
  resolve("clang_Cursor_isMacroBuiltin", clang_Cursor_isMacroBuiltin);
  resolve("clang_Cursor_isFunctionInlined", clang_Cursor_isFunctionInlined);
  resolve("clang_isVolatileQualifiedType", clang_isVolatileQualifiedType);
  resolve("clang_isRestrictQualifiedType", clang_isRestrictQualifiedType);
  resolve("clang_getAddressSpace", clang_getAddressSpace);
  resolve("clang_getTypedefName", clang_getTypedefName);
  resolve("clang_getPointeeType", clang_getPointeeType);
  resolve("clang_getTypeDeclaration", clang_getTypeDeclaration);
  resolve("clang_getDeclObjCTypeEncoding", clang_getDeclObjCTypeEncoding);
  resolve("clang_Type_getObjCEncoding", clang_Type_getObjCEncoding);
  resolve("clang_getTypeKindSpelling", clang_getTypeKindSpelling);
  resolve("clang_getFunctionTypeCallingConv", clang_getFunctionTypeCallingConv);
  resolve("clang_getResultType", clang_getResultType);
  resolve("clang_getExceptionSpecificationType", clang_getExceptionSpecificationType);
  resolve("clang_getNumArgTypes", clang_getNumArgTypes);
  resolve("clang_getArgType", clang_getArgType);
  resolve("clang_isFunctionTypeVariadic", clang_isFunctionTypeVariadic);
  resolve("clang_getCursorResultType", clang_getCursorResultType);
  resolve("clang_getCursorExceptionSpecificationType", clang_getCursorExceptionSpecificationType);
  resolve("clang_isPODType", clang_isPODType);
  resolve("clang_getElementType", clang_getElementType);
  resolve("clang_getNumElements", clang_getNumElements);
  resolve("clang_getArrayElementType", clang_getArrayElementType);
  resolve("clang_getArraySize", clang_getArraySize);
  resolve("clang_Type_getNamedType", clang_Type_getNamedType);
  resolve("clang_Type_isTransparentTagTypedef", clang_Type_isTransparentTagTypedef);
  resolve("clang_Type_getAlignOf", clang_Type_getAlignOf);
  resolve("clang_Type_getClassType", clang_Type_getClassType);
  resolve("clang_Type_getSizeOf", clang_Type_getSizeOf);
  resolve("clang_Type_getOffsetOf", clang_Type_getOffsetOf);
  resolve("clang_Cursor_getOffsetOfField", clang_Cursor_getOffsetOfField);
  resolve("clang_Cursor_isAnonymous", clang_Cursor_isAnonymous);
  resolve("clang_Type_getNumTemplateArguments", clang_Type_getNumTemplateArguments);
  resolve("clang_Type_getTemplateArgumentAsType", clang_Type_getTemplateArgumentAsType);
  resolve("clang_Type_getCXXRefQualifier", clang_Type_getCXXRefQualifier);
  resolve("clang_Cursor_isBitField", clang_Cursor_isBitField);
  resolve("clang_isVirtualBase", clang_isVirtualBase);
  resolve("clang_getCXXAccessSpecifier", clang_getCXXAccessSpecifier);
  resolve("clang_Cursor_getStorageClass", clang_Cursor_getStorageClass);
  resolve("clang_getNumOverloadedDecls", clang_getNumOverloadedDecls);
  resolve("clang_getOverloadedDecl", clang_getOverloadedDecl);
  resolve("clang_getIBOutletCollectionType", clang_getIBOutletCollectionType);
  resolve("clang_visitChildren", clang_visitChildren);
  resolve("clang_getCursorUSR", clang_getCursorUSR);
  resolve("clang_constructUSR_ObjCClass", clang_constructUSR_ObjCClass);
  resolve("clang_constructUSR_ObjCCategory", clang_constructUSR_ObjCCategory);
  resolve("clang_constructUSR_ObjCProtocol", clang_constructUSR_ObjCProtocol);
  resolve("clang_constructUSR_ObjCIvar", clang_constructUSR_ObjCIvar);
  resolve("clang_constructUSR_ObjCMethod", clang_constructUSR_ObjCMethod);
  resolve("clang_constructUSR_ObjCProperty", clang_constructUSR_ObjCProperty);
  resolve("clang_getCursorSpelling", clang_getCursorSpelling);
  resolve("clang_Cursor_getSpellingNameRange", clang_Cursor_getSpellingNameRange);
  resolve("clang_PrintingPolicy_getProperty", clang_PrintingPolicy_getProperty);
  resolve("clang_PrintingPolicy_setProperty", clang_PrintingPolicy_setProperty);
  resolve("clang_getCursorPrintingPolicy", clang_getCursorPrintingPolicy);
  resolve("clang_PrintingPolicy_dispose", clang_PrintingPolicy_dispose);
  resolve("clang_getCursorPrettyPrinted", clang_getCursorPrettyPrinted);
  resolve("clang_getCursorDisplayName", clang_getCursorDisplayName);
  resolve("clang_getCursorReferenced", clang_getCursorReferenced);
  resolve("clang_getCursorDefinition", clang_getCursorDefinition);
  resolve("clang_isCursorDefinition", clang_isCursorDefinition);
  resolve("clang_getCanonicalCursor", clang_getCanonicalCursor);
  resolve("clang_Cursor_getObjCSelectorIndex", clang_Cursor_getObjCSelectorIndex);
  resolve("clang_Cursor_isDynamicCall", clang_Cursor_isDynamicCall);
  resolve("clang_Cursor_getReceiverType", clang_Cursor_getReceiverType);
  resolve("clang_Cursor_getObjCPropertyAttributes", clang_Cursor_getObjCPropertyAttributes);
  resolve("clang_Cursor_getObjCDeclQualifiers", clang_Cursor_getObjCDeclQualifiers);
  resolve("clang_Cursor_isObjCOptional", clang_Cursor_isObjCOptional);
  resolve("clang_Cursor_isVariadic", clang_Cursor_isVariadic);
  resolve("clang_Cursor_isExternalSymbol", clang_Cursor_isExternalSymbol);
  resolve("clang_Cursor_getCommentRange", clang_Cursor_getCommentRange);
  resolve("clang_Cursor_getRawCommentText", clang_Cursor_getRawCommentText);
  resolve("clang_Cursor_getBriefCommentText", clang_Cursor_getBriefCommentText);
  resolve("clang_Cursor_getMangling", clang_Cursor_getMangling);
  resolve("clang_Cursor_getCXXManglings", clang_Cursor_getCXXManglings);
  resolve("clang_Cursor_getObjCManglings", clang_Cursor_getObjCManglings);
  resolve("clang_Cursor_getModule", clang_Cursor_getModule);
  resolve("clang_getModuleForFile", clang_getModuleForFile);
  resolve("clang_Module_getASTFile", clang_Module_getASTFile);
  resolve("clang_Module_getParent", clang_Module_getParent);
  resolve("clang_Module_getName", clang_Module_getName);
  resolve("clang_Module_getFullName", clang_Module_getFullName);
  resolve("clang_Module_isSystem", clang_Module_isSystem);
  resolve("clang_Module_getNumTopLevelHeaders", clang_Module_getNumTopLevelHeaders);
  resolve("clang_Module_getTopLevelHeader", clang_Module_getTopLevelHeader);
  resolve("clang_CXXConstructor_isConvertingConstructor", clang_CXXConstructor_isConvertingConstructor);
  resolve("clang_CXXConstructor_isCopyConstructor", clang_CXXConstructor_isCopyConstructor);
  resolve("clang_CXXConstructor_isDefaultConstructor", clang_CXXConstructor_isDefaultConstructor);
  resolve("clang_CXXConstructor_isMoveConstructor", clang_CXXConstructor_isMoveConstructor);
  resolve("clang_CXXField_isMutable", clang_CXXField_isMutable);
  resolve("clang_CXXMethod_isDefaulted", clang_CXXMethod_isDefaulted);
  resolve("clang_CXXMethod_isPureVirtual", clang_CXXMethod_isPureVirtual);
  resolve("clang_CXXMethod_isVirtual", clang_CXXMethod_isVirtual);
  resolve("clang_CXXMethod_isStatic", clang_CXXMethod_isStatic);
  resolve("clang_CXXRecord_isAbstract", clang_CXXRecord_isAbstract);
  resolve("clang_EnumDecl_isScoped", clang_EnumDecl_isScoped);
  resolve("clang_CXXMethod_isConst", clang_CXXMethod_isConst);
  resolve("clang_getTemplateCursorKind", clang_getTemplateCursorKind);
  resolve("clang_getSpecializedCursorTemplate", clang_getSpecializedCursorTemplate);
  resolve("clang_getCursorReferenceNameRange", clang_getCursorReferenceNameRange);

  resolve("clang_getToken", clang_getToken);
  resolve("clang_getTokenKind", clang_getTokenKind);
  resolve("clang_getTokenSpelling", clang_getTokenSpelling);
  resolve("clang_getTokenLocation", clang_getTokenLocation);
  resolve("clang_getTokenExtent", clang_getTokenExtent);
  resolve("clang_tokenize", clang_tokenize);
  resolve("clang_annotateTokens", clang_annotateTokens);
  resolve("clang_disposeTokens", clang_disposeTokens);
  resolve("clang_getCursorKindSpelling", clang_getCursorKindSpelling);
  resolve("clang_getDefinitionSpellingAndExtent", clang_getDefinitionSpellingAndExtent);
  resolve("clang_enableStackTraces", clang_enableStackTraces);
  resolve("clang_executeOnThread", clang_executeOnThread);
  resolve("clang_toggleCrashRecovery", clang_toggleCrashRecovery);

  resolve("clang_getInclusions", clang_getInclusions);

  resolve("clang_Cursor_Evaluate", clang_Cursor_Evaluate);
  resolve("clang_EvalResult_getKind", clang_EvalResult_getKind);
  resolve("clang_EvalResult_getAsInt", clang_EvalResult_getAsInt);
  resolve("clang_EvalResult_getAsLongLong", clang_EvalResult_getAsLongLong);
  resolve("clang_EvalResult_isUnsignedInt", clang_EvalResult_isUnsignedInt);
  resolve("clang_EvalResult_getAsUnsigned", clang_EvalResult_getAsUnsigned);
  resolve("clang_EvalResult_getAsDouble", clang_EvalResult_getAsDouble);
  resolve("clang_EvalResult_getAsStr", clang_EvalResult_getAsStr);
  resolve("clang_EvalResult_dispose", clang_EvalResult_dispose);

  resolve("clang_getRemappings", clang_getRemappings);
  resolve("clang_getRemappingsFromFileList", clang_getRemappingsFromFileList);
  resolve("clang_remap_getNumFiles", clang_remap_getNumFiles);
  resolve("clang_remap_getFilenames", clang_remap_getFilenames);
  resolve("clang_remap_dispose", clang_remap_dispose);

  resolve("clang_findReferencesInFile", clang_findReferencesInFile);
  resolve("clang_findIncludesInFile", clang_findIncludesInFile);

  resolve("clang_index_isEntityObjCContainerKind", clang_index_isEntityObjCContainerKind);
  resolve("clang_index_getObjCContainerDeclInfo", clang_index_getObjCContainerDeclInfo);
  resolve("clang_index_getObjCInterfaceDeclInfo", clang_index_getObjCInterfaceDeclInfo);
  resolve("clang_index_getObjCCategoryDeclInfo", clang_index_getObjCCategoryDeclInfo);
  resolve("clang_index_getObjCProtocolRefListInfo", clang_index_getObjCProtocolRefListInfo);
  resolve("clang_index_getObjCPropertyDeclInfo", clang_index_getObjCPropertyDeclInfo);
  resolve("clang_index_getIBOutletCollectionAttrInfo", clang_index_getIBOutletCollectionAttrInfo);
  resolve("clang_index_getCXXClassDeclInfo", clang_index_getCXXClassDeclInfo);
  resolve("clang_index_getClientContainer", clang_index_getClientContainer);
  resolve("clang_index_setClientContainer", clang_index_setClientContainer);
  resolve("clang_index_getClientEntity", clang_index_getClientEntity);
  resolve("clang_index_setClientEntity", clang_index_setClientEntity);

  resolve("clang_IndexAction_create", clang_IndexAction_create);
  resolve("clang_IndexAction_dispose", clang_IndexAction_dispose);
  resolve("clang_indexSourceFile", clang_indexSourceFile);
  resolve("clang_indexSourceFileFullArgv", clang_indexSourceFileFullArgv);
  resolve("clang_indexTranslationUnit", clang_indexTranslationUnit);
  resolve("clang_indexLoc_getFileLocation", clang_indexLoc_getFileLocation);
  resolve("clang_indexLoc_getCXSourceLocation", clang_indexLoc_getCXSourceLocation);
  resolve("clang_Type_visitFields", clang_Type_visitFields);
}

LibClangParser::~LibClangParser()
{

}

CXVersion LibClangParser::version() const
{
  return m_version;
}

const std::string& LibClangParser::printableVersion() const
{
  return m_printable_version;
}

std::string LibClangParser::toStdString(CXString str)
{
  std::string result = clang_getCString(str);
  clang_disposeString(str);
  return result;
}

std::string LibClangParser::getCursorFile(CXCursor cursor)
{
  CXSourceLocation location = clang_getCursorLocation(cursor);

  CXFile file;
  unsigned int line, col, offset;
  clang_getSpellingLocation(location, &file, &line, &col, &offset);

  std::string result = toStdString(clang_getFileName(file));

  for (char& c : result)
  {
    if (c == '\\')
      c = '/';
  }

  return result;
}

std::string LibClangParser::getCursorSpelling(CXCursor cursor)
{
  return toStdString(clang_getCursorSpelling(cursor));
}

bool LibClangParser::isForwardDeclaration(CXCursor cursor)
{
  return clang_equalCursors(clang_getCursorDefinition(cursor), clang_getNullCursor());
}

} // namespace parsers

} // namespace cxx
