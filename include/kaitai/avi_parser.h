#include "parser/parser.h"
#include "kaitai/avi.h"
namespace veles {
namespace kaitai {
class AviParser : public parser::Parser {
public:
    AviParser() : parser::Parser("avi (ksy)") {}
    void parse(dbif::ObjectHandle blob, uint64_t start = 0, 
    dbif::ObjectHandle parent_chunk = dbif::ObjectHandle()) override {
        auto stream = kaitai::kstream(blob, start, parent_chunk);
        auto parser = kaitai::avi::avi_t(&stream);
    }
};

} // namespace kaitai
} // namespace veles
