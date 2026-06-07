#include  "./utils.hpp"

std::string Utils::normalise_identifier(std::string identifier)
{
  if (identifier.front() == '$')
  {
    identifier.erase(0, 1);
  }

  return identifier;
}
