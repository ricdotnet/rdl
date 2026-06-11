
template<typename T>
std::shared_ptr<T> native(const Value &v)
{
  return std::static_pointer_cast<T>(v.object.native_object);
}
