#include <iostream>
#include <cctype>
#include <stdexcept>

#include "DatatypeStore.h"
#include "FieldStore.h"
#include "HelperFunctions.h"
#include "TypeCheck.h"


namespace kx {
namespace arrowkdb {

template <>
GenericStore<std::shared_ptr<arrow::DataType>>* GenericStore<std::shared_ptr<arrow::DataType>>::instance = nullptr;

GenericStore<std::shared_ptr<arrow::DataType>>* GetDatatypeStore()
{
  return GenericStore<std::shared_ptr<arrow::DataType>>::Instance();
}

arrow::TimeUnit::type ToTimeUnit(const std::string& tu_str)
{
  std::string upper;
  for (auto i : tu_str)
    upper.push_back((unsigned char)std::toupper(i));

  if (upper == "SECOND" || upper == "S")
    return arrow::TimeUnit::SECOND;
  else if (upper == "MILLI" || upper == "MS")
    return arrow::TimeUnit::MILLI;
  else if (upper == "MICRO" || upper == "US")
    return arrow::TimeUnit::MICRO;
  else if (upper == "NANO" || upper == "NS")
    return arrow::TimeUnit::NANO;
 
  throw kx::arrowkdb::TypeCheck("Invalid TimeUnit");
}

const std::string FromTimeUnit(arrow::TimeUnit::type tu)
{
  switch (tu) {
  case arrow::TimeUnit::SECOND:
    return "SECOND";
  case arrow::TimeUnit::MILLI:
    return "MILLI";
  case arrow::TimeUnit::MICRO:
    return "MICRO";
  case arrow::TimeUnit::NANO:
    return "NANO";
  default:
    throw kx::arrowkdb::TypeCheck("Invalid TimeUnit");
  }
}

} // namespace arrowkdb
} // namespace kx


K listDatatypes(K unused)
{
  auto datatype_ids = kx::arrowkdb::GetDatatypeStore()->List();
  
  K result = ktn(KI, datatype_ids.size());
  size_t index = 0;
  for (auto it : datatype_ids)
    kI(result)[index++] = it;

  return result;
}

K printDatatype(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return kp((S)datatype->ToString().c_str());
}

K removeDatatype(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Remove(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return (K)0;
}

K equalDatatypes(K first_datatype_id, K second_datatype_id)
{
  if (first_datatype_id->t != -KI)
    return krr((S)"first_datatype_id not -6h");
  if (second_datatype_id->t != -KI)
    return krr((S)"second_datatype_id not -6h");

  auto first_datatype = kx::arrowkdb::GetDatatypeStore()->Find(first_datatype_id->i);
  if (!first_datatype)
    return krr((S)"first datatype not found");
  auto second_datatype = kx::arrowkdb::GetDatatypeStore()->Find(second_datatype_id->i);
  if (!second_datatype)
    return krr((S)"second datatype not found");

  return kb(first_datatype->Equals(second_datatype));
}

K datatypeName(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return ks((S)datatype->name().c_str());
}

K null(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::null()));
}

K boolean(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::boolean()));
}

K uint8(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::uint8()));
}

K int8(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::int8()));
}

K uint16(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::uint16()));
}

K int16(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::int16()));
}

K uint32(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::uint32()));
}

K int32(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::int32()));
}

K uint64(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::uint64()));
}

K int64(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::int64()));
}

K float16(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::float16()));
}

K float32(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::float32()));
}

K float64(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::float64()));
}

K utf8(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::utf8()));
}

K large_utf8(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::large_utf8()));
}

K binary(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::binary()));
}

K large_binary(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::large_binary()));
}

K fixed_size_binary(K byte_width)
{
  if (byte_width->t != -KI)
    return krr((S)"byte_width not -6h");
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::fixed_size_binary(byte_width->i)));
}

K getByteWidth(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::FIXED_SIZE_BINARY:
  {
    auto fsb_type = std::static_pointer_cast<arrow::FixedSizeBinaryType>(datatype);
    return ki(fsb_type->byte_width());
  }
  default:
    return krr((S)"Not FIXED_SIZE_BINARY datatype");
  }
}

K date32(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::date32()));
}

K date64(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::date64()));
}

K timestamp(K time_unit)
{
  if (!kx::arrowkdb::IsKdbString(time_unit))
    return krr((S)"time_unit not -11|10h");

  KDB_EXCEPTION_TRY;

  auto tu = kx::arrowkdb::ToTimeUnit(kx::arrowkdb::GetKdbString(time_unit));
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::timestamp(tu)));
  
  KDB_EXCEPTION_CATCH;
}

K time32(K time_unit)
{
  if (!kx::arrowkdb::IsKdbString(time_unit))
    return krr((S)"time_unit not -11|10h");

  KDB_EXCEPTION_TRY;

  auto tu = kx::arrowkdb::ToTimeUnit(kx::arrowkdb::GetKdbString(time_unit));
  if (tu == arrow::TimeUnit::SECOND || tu == arrow::TimeUnit::MILLI)
    return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::time32(tu)));
  else
    return krr((S)"time32 time_unit not SECOND or MILLI");

  KDB_EXCEPTION_CATCH;
}

K time64(K time_unit)
{
  if (!kx::arrowkdb::IsKdbString(time_unit))
    return krr((S)"time_unit not -11|10h");

  KDB_EXCEPTION_TRY;

  auto tu = kx::arrowkdb::ToTimeUnit(kx::arrowkdb::GetKdbString(time_unit));
  if (tu == arrow::TimeUnit::MICRO || tu == arrow::TimeUnit::NANO)
    return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::time64(tu)));
  else
    return krr((S)"time64 time_unit not MICRO or NANO");

  KDB_EXCEPTION_CATCH;
}

K month_interval(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::month_interval()));
}

K day_time_interval(K unused)
{
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::day_time_interval()));
}

K duration(K time_unit)
{
  if (!kx::arrowkdb::IsKdbString(time_unit))
    return krr((S)"time_unit not -11|10h");

  KDB_EXCEPTION_TRY;

  auto tu = kx::arrowkdb::ToTimeUnit(kx::arrowkdb::GetKdbString(time_unit));
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::duration(tu)));

  KDB_EXCEPTION_CATCH;
}

K getTimeUnit(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  KDB_EXCEPTION_TRY;

  switch (datatype->id()) {
  case arrow::Type::TIMESTAMP:
  {
    auto ts_type = std::static_pointer_cast<arrow::TimestampType>(datatype);
    auto tu_str = kx::arrowkdb::FromTimeUnit(ts_type->unit());
    return ks((S)tu_str.c_str());
  }
  case arrow::Type::TIME32:
  {
    auto t32_type = std::static_pointer_cast<arrow::Time32Type>(datatype);
    auto tu_str = kx::arrowkdb::FromTimeUnit(t32_type->unit());
    return ks((S)tu_str.c_str());
  }
  case arrow::Type::TIME64:
  {
    auto t64_type = std::static_pointer_cast<arrow::Time64Type>(datatype);
    auto tu_str = kx::arrowkdb::FromTimeUnit(t64_type->unit());
    return ks((S)tu_str.c_str());
  }
  case arrow::Type::DURATION:
  {
    auto dur_type = std::static_pointer_cast<arrow::DurationType>(datatype);
    auto tu_str = kx::arrowkdb::FromTimeUnit(dur_type->unit());
    return ks((S)tu_str.c_str());
  }
  default:
    return krr((S)"Not timeunit datatype");
  }
  KDB_EXCEPTION_CATCH;
}

K decimal128(K precision, K scale)
{
  if (precision->t != -KI)
    return krr((S)"precision not -6h");
  if (scale->t != -KI)
    return krr((S)"scale not -6h");

  if (precision->i < 1 || precision->i > 38)
    return krr((S)"precision out of range, required: 1 <= precision <= 38");

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::smallest_decimal(precision->i, scale->i)));
}

K getPrecisionScale(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::DECIMAL:
  {
    auto dec_type = std::static_pointer_cast<arrow::Decimal128Type>(datatype);
    // Could be simple list but wouldn't match decimal() constructor args
    return knk(2, ki(dec_type->precision()), ki(dec_type->scale()));
  }
  default:
    return krr((S)"Not decimal datatype");
  }
}

K list(K child_datatype_id)
{
  if (child_datatype_id->t != -KI)
    return krr((S)"child_datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(child_datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::list(datatype)));
}

K large_list(K child_datatype_id)
{
  if (child_datatype_id->t != -KI)
    return krr((S)"child_datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(child_datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::large_list(datatype)));
}

K fixed_size_list(K child_datatype_id, K list_size)
{
  if (child_datatype_id->t != -KI)
    return krr((S)"child_datatype_id not -6h");
  if (list_size->t != -KI)
    return krr((S)"list_size not -KI");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(child_datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::fixed_size_list(datatype, list_size->i)));
}

K getListDatatype(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::LIST:
  case arrow::Type::LARGE_LIST:
  case arrow::Type::FIXED_SIZE_LIST:
  {
    auto list_type = std::static_pointer_cast<arrow::BaseListType>(datatype);
    return ki(kx::arrowkdb::GetDatatypeStore()->ReverseFind(list_type->value_type()));
  }
  default:
    return krr((S)"Not list datatype");
  }
}

K getListSize(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::FIXED_SIZE_LIST:
  {
    auto fsl_type = std::static_pointer_cast<arrow::FixedSizeListType>(datatype);
    return ki(fsl_type->list_size());
  }
  default:
    return krr((S)"Not FIXED_SIZE_LIST datatype");
  }
}

K map(K key_datatype_id, K item_datatype_id)
{
  if (key_datatype_id->t != -KI)
    return krr((S)"key_datatype_id not -6h");
  if (item_datatype_id->t != -KI)
    return krr((S)"item_datatype_id not -6h");

  auto key_datatype = kx::arrowkdb::GetDatatypeStore()->Find(key_datatype_id->i);
  if (!key_datatype)
    return krr((S)"key datatype not found");
  auto item_datatype = kx::arrowkdb::GetDatatypeStore()->Find(item_datatype_id->i);
  if (!item_datatype)
    return krr((S)"item datatype not found");

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::map(key_datatype, item_datatype)));
}

K getMapDatatypes(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::MAP:
  {
    auto map_type = std::static_pointer_cast<arrow::MapType>(datatype);
    // Could be simple list but wouldn't match map() constructor args
    K key_datatype_id = ki(kx::arrowkdb::GetDatatypeStore()->ReverseFind(map_type->key_type()));
    K value_datatype_id = ki(kx::arrowkdb::GetDatatypeStore()->ReverseFind(map_type->item_type()));
    return knk(2, key_datatype_id, value_datatype_id);
  }
  default:
    return krr((S)"Not map datatype");
  }
}

K dictionary(K value_datatype_id, K index_datatype_id)
{
  KDB_EXCEPTION_TRY;

  if (value_datatype_id->t != -KI)
    return krr((S)"value_datatype_id not -6h");
  if (index_datatype_id->t != -KI)
    return krr((S)"index_datatype_id not -6h");

  auto value_datatype = kx::arrowkdb::GetDatatypeStore()->Find(value_datatype_id->i);
  if (!value_datatype)
    return krr((S)"value datatype not found");
  auto index_datatype = kx::arrowkdb::GetDatatypeStore()->Find(index_datatype_id->i);
  if (!index_datatype)
    return krr((S)"index datatype not found");

  std::shared_ptr<arrow::DataType> datatype;
  PARQUET_ASSIGN_OR_THROW(datatype, arrow::DictionaryType::Make(index_datatype, value_datatype));

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(datatype));

  KDB_EXCEPTION_CATCH;
}

K getDictionaryDatatypes(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  switch (datatype->id()) {
  case arrow::Type::DICTIONARY:
  {
    auto map_type = std::static_pointer_cast<arrow::DictionaryType>(datatype);
    // Could be simple list but wouldn't match map() constructor args
    K value_datatype_id = ki(kx::arrowkdb::GetDatatypeStore()->ReverseFind(map_type->value_type()));
    K index_datatype_id = ki(kx::arrowkdb::GetDatatypeStore()->ReverseFind(map_type->index_type()));
    return knk(2, value_datatype_id, index_datatype_id);
  }
  default:
    return krr((S)"Not dictionary datatype");
  }
}

// Function cannot be called 'struct' since that is a C++ reserved keyword
K struct_(K field_ids)
{
  if (field_ids->t != KI)
    return krr((S)"field_ids not 6h");

  arrow::FieldVector field_vector;
  for (auto i = 0; i < field_ids->n; ++i) {
    auto field = kx::arrowkdb::GetFieldStore()->Find(kI(field_ids)[i]);
    if (!field)
      return krr((S)"field not found");
    field_vector.push_back(field);
  }

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::struct_(field_vector)));
}

K sparse_union(K field_ids)
{
  if (field_ids->t != KI)
    return krr((S)"field_ids not 6h");

  arrow::FieldVector field_vector;
  for (auto i = 0; i < field_ids->n; ++i) {
    auto field = kx::arrowkdb::GetFieldStore()->Find(kI(field_ids)[i]);
    if (!field)
      return krr((S)"field not found");
    field_vector.push_back(field);
  }

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::sparse_union(field_vector)));
}

K dense_union(K field_ids)
{
  if (field_ids->t != KI)
    return krr((S)"field_ids not 6h");

  arrow::FieldVector field_vector;
  for (auto i = 0; i < field_ids->n; ++i) {
    auto field = kx::arrowkdb::GetFieldStore()->Find(kI(field_ids)[i]);
    if (!field)
      return krr((S)"field not found");
    field_vector.push_back(field);
  }

  return ki(kx::arrowkdb::GetDatatypeStore()->Add(arrow::dense_union(field_vector)));
}

K getChildFields(K datatype_id)
{
  if (datatype_id->t != -KI)
    return krr((S)"datatype_id not -6h");

  auto datatype = kx::arrowkdb::GetDatatypeStore()->Find(datatype_id->i);
  if (!datatype)
    return krr((S)"datatype not found");

  K result = ktn(KI, datatype->num_fields());
  for (auto i = 0; i < datatype->num_fields(); ++i) {
    auto child = datatype->field(i);
    kI(result)[i] = kx::arrowkdb::GetFieldStore()->ReverseFind(child);
  }

  return result;
}

K inferDatatype(K k_array)
{
  KDB_EXCEPTION_TRY;

  auto datatype = kx::arrowkdb::GetArrowType(k_array);
  return ki(kx::arrowkdb::GetDatatypeStore()->Add(datatype));

  KDB_EXCEPTION_CATCH;
}
