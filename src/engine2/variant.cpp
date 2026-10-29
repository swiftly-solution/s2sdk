#include "public/entity2/variant.h"

const char *VariantFieldTypeName( fieldtype_t eType )
{
	switch(eType)
	{
		case FIELD_VOID:					return "void";
		case FIELD_FLOAT32:					return "float32";
		case FIELD_STRING:					return "string_t";
		case FIELD_VECTOR:					return "vector";
		case FIELD_QUATERNION:				return "quaternion";
		case FIELD_INT32:					return "int32";
		case FIELD_BOOLEAN:					return "boolean";
		case FIELD_CHARACTER:				return "character";
		case FIELD_COLOR32:					return "color";
		case FIELD_EHANDLE:					return "ehandle";
		case FIELD_POSITION_VECTOR:			return "vectorws";
		case FIELD_VECTOR2D:				return "vector2d";
		case FIELD_VECTOR4D:				return "vector4d";
		case FIELD_INT64:					return "int64";
		case FIELD_RESOURCE:				return "resourcehandle";
		case FIELD_CSTRING:					return "cstring";
		case FIELD_HSCRIPT:					return "hscript";
		case FIELD_VARIANT:					return "variant";
		case FIELD_UINT64:					return "uint64";
		case FIELD_FLOAT64:					return "float64";
		case FIELD_UINT32:					return "unsigned";
		case FIELD_UTLSTRINGTOKEN:			return "utlstringtoken";
		case FIELD_QANGLE:					return "qangle";
		case FIELD_HSCRIPT_LIGHTBINDING:	return "hscript_lightbinding";
		case FIELD_V8_VALUE:				return "js_value";
		case FIELD_V8_OBJECT:				return "js_object";
		case FIELD_V8_ARRAY:				return "js_array";
		case FIELD_V8_CALLBACK_INFO:		return "js_raw_args";
		case FIELD_UINT8:					return "uint8";
		case FIELD_GLOBALSYMBOL:			return "globalsymbol";
		default:							return "unknown_variant_type";
	}
}