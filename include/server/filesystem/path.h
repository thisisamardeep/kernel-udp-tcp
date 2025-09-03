#ifndef ASIOUTILS_FILESYSTEM_PATH_H
#define ASIOUTILS_FILESYSTEM_PATH_H

#include "encoding.h"
#include "timestamp.h"
#include "flags.h"
#include "format.h"

#include <string>

namespace AsioUtils {

enum class FileType
{
    NONE,
    REGULAR,
    DIRECTORY,
    SYMLINK,
    BLOCK,
    CHARACTER,
    FIFO,
    SOCKET,
    UNKNOWN
};

enum class FileAttributes
{
    NONE      = 0x00,
    NORMAL    = 0x01,
    ARCHIVED  = 0x02,
    HIDDEN    = 0x04,
    INDEXED   = 0x08,
    OFFLINE   = 0x10,
    READONLY  = 0x20,
    SYSTEM    = 0x40,
    TEMPORARY = 0x80
};

enum class FilePermissions
{
    NONE      = 00000,
    IRUSR     = 00400,
    IWUSR     = 00200,
    IXUSR     = 00100,
    IRWXU     = 00700,
    IRGRP     = 00040,
    IWGRP     = 00020,
    IXGRP     = 00010,
    IRWXG     = 00070,
    IROTH     = 00004,
    IWOTH     = 00002,
    IXOTH     = 00001,
    IRWXO     = 00007,
    ISUID     = 04000,
    ISGID     = 02000,
    ISVTX     = 01000
};

struct SpaceInfo
{
    uint64_t capacity;
    uint64_t free;
    uint64_t available;
};

class Path
{
public:
    Path() : _path() {}
    Path(const char* path) : _path(path) {}
    Path(const std::string& path) : _path(path) {}
    Path(const wchar_t* path) : _path(Encoding::ToUTF8(path)) {}
    Path(const std::wstring& path) : _path(Encoding::ToUTF8(path)) {}
    Path(const Path&) = default;
    Path(Path&&) = default;
    ~Path() = default;

    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) = default;

    explicit operator bool() const noexcept { return !empty(); }

    Path& operator/=(const Path& path)
    { return Append(path); }
    friend Path operator/(const Path& path1, const Path& path2)
    { return Path(path1).Append(path2); }

    Path& operator+=(const Path& path)
    { return Concat(path); }
    friend Path operator+(const Path& path1, const Path& path2)
    { return Path(path1).Concat(path2); }

    friend bool operator==(const Path& path1, const Path& path2)
    { return path1._path == path2._path; }
    friend bool operator!=(const Path& path1, const Path& path2)
    { return path1._path != path2._path; }
    friend bool operator<(const Path& path1, const Path& path2)
    { return path1._path < path2._path; }
    friend bool operator>(const Path& path1, const Path& path2)
    { return path1._path > path2._path; }
    friend bool operator<=(const Path& path1, const Path& path2)
    { return path1._path <= path2._path; }
    friend bool operator>=(const Path& path1, const Path& path2)
    { return path1._path >= path2._path; }

    const std::string& string() const noexcept { return _path; }
    std::wstring wstring() const { return Encoding::FromUTF8(_path); }

    Path root() const;
    Path relative() const;
    Path parent() const;
    Path filename() const;
    Path stem() const;
    Path extension() const;

    Path absolute() const;
    Path canonical() const;
    Path validate(char placeholder = '_') const;

    FileType type() const;
    Flags<FileAttributes> attributes() const;
    Flags<FilePermissions> permissions() const;
    UtcTimestamp created() const;
    UtcTimestamp modified() const;
    size_t hardlinks() const;
    SpaceInfo space() const;

    bool empty() const noexcept { return _path.empty(); }

    bool HasRoot() const { return !root().empty(); }
    bool HasRelative() const { return !relative().empty(); }
    bool HasParent() const { return !parent().empty(); }
    bool HasFilename() const { return !filename().empty(); }
    bool HasStem() const { return !stem().empty(); }
    bool HasExtension() const { return !extension().empty(); }

    bool IsAbsolute() const { return HasRoot(); }
    bool IsRelative() const { return !HasRoot(); }

    bool IsExists() const { return type() != FileType::NONE; }

    bool IsRegularFile() const { return type() == FileType::REGULAR; }
    bool IsDirectory() const { return type() == FileType::DIRECTORY; }
    bool IsSymlink() const { return type() == FileType::SYMLINK; }
    bool IsOther() const;

    bool IsEquivalent(const Path& path) const;

    void Clear() noexcept { return _path.clear(); }

    Path& Assign(const Path& path);
    Path& Append(const Path& path);
    Path& Concat(const Path& path);
    Path& MakePreferred();
    Path& ReplaceFilename(const Path& filename);
    Path& ReplaceExtension(const Path& extension);

    Path& RemoveFilename() { return ReplaceFilename(""); }
    Path& RemoveExtension() { return ReplaceExtension(""); }
    Path& RemoveTrailingSeparators();

    static bool deprecated(char character) noexcept;
    static bool deprecated(wchar_t character) noexcept;
    static std::string deprecated();

    static char separator() noexcept;

    static Path initial();
    static Path current();
    static Path executable();
    static Path home();
    static Path temp();
    static Path unique();

    static Path Copy(const Path& src, const Path& dst, bool overwrite = false);
    static Path CopyIf(const Path& src, const Path& dst, const std::string& pattern = "", bool overwrite = false);
    static Path CopyAll(const Path& src, const Path& dst, bool overwrite = false);
    static Path Rename(const Path& src, const Path& dst);
    static Path Remove(const Path& path);
    static Path RemoveAll(const Path& path);
    static Path RemoveIf(const Path& path, const std::string& pattern = "");

    static void SetAttributes(const Path& path, const Flags<FileAttributes>& attributes);
    static void SetPermissions(const Path& path, const Flags<FilePermissions>& permissions);
    static void SetCreated(const Path& path, const UtcTimestamp& timestamp);
    static void SetModified(const Path& path, const UtcTimestamp& timestamp);

    static void SetCurrent(const Path& path);

    static void Touch(const Path& path);

    friend std::istream& operator>>(std::istream& is, Path& path)
    { is >> path._path; return is; }
    friend std::ostream& operator<<(std::ostream& os, const Path& path)
    { os << path._path; return os; }

    void swap(Path& path) noexcept;
    friend void swap(Path& path1, Path& path2) noexcept;

protected:
    std::string _path;
};

}

ENUM_FLAGS(AsioUtils::FileAttributes)
ENUM_FLAGS(AsioUtils::FilePermissions)

namespace AsioUtils {

    inline bool Path::IsOther() const
    {
        FileType t = type();
        return ((t != FileType::NONE) && (t != FileType::REGULAR) && (t != FileType::DIRECTORY) && (t != FileType::SYMLINK));
    }

    inline Path& Path::Assign(const Path& path)
    {
        _path = path._path;
        return *this;
    }

    inline Path& Path::Concat(const Path& path)
    {
        _path.append(path._path);
        return *this;
    }

    inline void Path::swap(Path& path) noexcept
    {
        using std::swap;
        swap(_path, path._path);
    }

    inline void swap(Path& path1, Path& path2) noexcept
    {
        path1.swap(path2);
    }

}

template <>
struct fmt::formatter<AsioUtils::Path> : formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(const AsioUtils::Path& value, FormatContext& ctx) const
    {
        return formatter<string_view>::format(value.string(), ctx);
    }
};

#include "filesystem/exceptions.h"

#endif