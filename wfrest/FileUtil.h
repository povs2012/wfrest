#ifndef WFREST_FILEUTIL_H_
#define WFREST_FILEUTIL_H_

#include <cstddef>
#include <string>
#include "wfrest/Macro.h"

namespace wfrest
{
    
class FileUtil
{
public:
    static int size(const std::string &path, OUT size_t *size);
    
};

} // namespace wfrest


#endif // WFREST_FILEUTIL_H_
