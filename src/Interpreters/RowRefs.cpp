#include <Interpreters/RowRefs.h>

#include <base/types.h>
#include <Columns/IColumn.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int BAD_TYPE_OF_FIELD;
}

namespace
{

/// maps enum values to types
template <typename F>
void callWithType(TypeIndex which, F && f)
{
    switch (which)
    {
        case TypeIndex::UInt8:  return f(UInt8());
        case TypeIndex::UInt16: return f(UInt16());
        case TypeIndex::UInt32: return f(UInt32());
        case TypeIndex::UInt64: return f(UInt64());
        case TypeIndex::Int8:   return f(Int8());
        case TypeIndex::Int16:  return f(Int16());
        case TypeIndex::Int32:  return f(Int32());
        case TypeIndex::Int64:  return f(Int64());
        case TypeIndex::Float32: return f(Float32());
        case TypeIndex::Float64: return f(Float64());
        case TypeIndex::Decimal32: return f(Decimal32());
        case TypeIndex::Decimal64: return f(Decimal64());
        case TypeIndex::Decimal128: return f(Decimal128());
        case TypeIndex::DateTime64: return f(DateTime64());
        default:
            break;
    }

    __builtin_unreachable();
}

}

AsofRowRefs createAsofRowRef(TypeIndex type, ASOF::Inequality inequality)
{
    AsofRowRefs a;
    auto call = [&](const auto & t)
    {
        using T = std::decay_t<decltype(t)>;
        switch (inequality)
        {
            case ASOF::Inequality::LessOrEquals:
                a = std::make_unique<SortedLookupVector<T, ASOF::Inequality::LessOrEquals>>();
                break;
            case ASOF::Inequality::Less:
                a = std::make_unique<SortedLookupVector<T, ASOF::Inequality::Less>>();
                break;
            case ASOF::Inequality::GreaterOrEquals:
                a = std::make_unique<SortedLookupVector<T, ASOF::Inequality::GreaterOrEquals>>();
                break;
            case ASOF::Inequality::Greater:
                a = std::make_unique<SortedLookupVector<T, ASOF::Inequality::Greater>>();
                break;
            default:
                throw Exception("Invalid ASOF Join order", ErrorCodes::LOGICAL_ERROR);
        }
    };

    callWithType(type, call);
    return a;
}

std::optional<TypeIndex> SortedLookupVectorBase::getTypeSize(const IColumn & asof_column, size_t & size)
{
    TypeIndex idx = asof_column.getDataType();

    switch (idx)
    {
        case TypeIndex::UInt8:
            size = sizeof(UInt8);
            return idx;
        case TypeIndex::UInt16:
            size = sizeof(UInt16);
            return idx;
        case TypeIndex::UInt32:
            size = sizeof(UInt32);
            return idx;
        case TypeIndex::UInt64:
            size = sizeof(UInt64);
            return idx;
        case TypeIndex::Int8:
            size = sizeof(Int8);
            return idx;
        case TypeIndex::Int16:
            size = sizeof(Int16);
            return idx;
        case TypeIndex::Int32:
            size = sizeof(Int32);
            return idx;
        case TypeIndex::Int64:
            size = sizeof(Int64);
            return idx;
        case TypeIndex::Float32:
            size = sizeof(Float32);
            return idx;
        case TypeIndex::Float64:
            size = sizeof(Float64);
            return idx;
        case TypeIndex::Decimal32:
            size = sizeof(Decimal32);
            return idx;
        case TypeIndex::Decimal64:
            size = sizeof(Decimal64);
            return idx;
        case TypeIndex::Decimal128:
            size = sizeof(Decimal128);
            return idx;
        case TypeIndex::DateTime64:
            size = sizeof(DateTime64);
            return idx;
        default:
            break;
    }

    throw Exception("ASOF join not supported for type: " + std::string(asof_column.getFamilyName()), ErrorCodes::BAD_TYPE_OF_FIELD);
}

}
