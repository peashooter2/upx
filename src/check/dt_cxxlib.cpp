/* dt_cxxlib.cpp -- doctest check

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2024 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   UPX and the UCL library are free software; you can redistribute them
   and/or modify them under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
 */

// lots of tests (and probably quite a number of redundant tests)
// modern compilers will optimize away much of this code

#include "../util/system_headers.h"
#include <vector> // std::vector
#include "../conf.h"

/*************************************************************************
// compile-time checks
**************************************************************************/

static_assert(upx::is_same_all_v<int>);
static_assert(upx::is_same_all_v<int, int>);
static_assert(upx::is_same_all_v<int, int, int>);
static_assert(!upx::is_same_all_v<int, char>);
static_assert(!upx::is_same_all_v<int, char, int>);
static_assert(!upx::is_same_all_v<int, int, char>);

static_assert(!upx::is_same_any_v<int>);
static_assert(upx::is_same_any_v<int, int>);
static_assert(upx::is_same_any_v<int, char, int>);
static_assert(upx::is_same_any_v<int, int, char>);
static_assert(!upx::is_same_any_v<int, char>);
static_assert(!upx::is_same_any_v<int, char, char>);
static_assert(!upx::is_same_any_v<int, char, long>);

static_assert(upx::is_same_any_v<ptrdiff_t, int, long, long long>);
static_assert(upx::is_same_any_v<size_t, unsigned, unsigned long, unsigned long long>);
// TODO later: CHERI
static_assert(upx::is_same_any_v<upx_uintptr_t, unsigned, unsigned long, unsigned long long>);

static_assert(usizeof(int) == sizeof(int));
static_assert(usizeof('a') == sizeof(char));
static_assert(usizeof("") == 1);
static_assert(usizeof("a") == 2);
static_assert(usizeof("ab") == 3);
static_assert(usizeof(L'a') == sizeof(wchar_t));
static_assert(usizeof(L"") == 1 * sizeof(wchar_t));
static_assert(usizeof(L"a") == 2 * sizeof(wchar_t));
static_assert(usizeof(L"ab") == 3 * sizeof(wchar_t));
static_assert(usizeof(0) == sizeof(int));
static_assert(usizeof(0L) == sizeof(long));
static_assert(usizeof(0LL) == sizeof(long long));
static_assert(usizeof(nullptr) == sizeof(void *));
static_assert(usizeof(sizeof(0)) == sizeof(size_t));
static_assert(usizeof(usizeof(0)) == sizeof(unsigned));
#if 0
// works, but may trigger clang/gcc warnings "-Wunused-value"
static_assert(usizeof((1LL, 1)) == sizeof(int));
static_assert(usizeof((1, 1LL)) == sizeof(long long));
#endif

static_assert(upx::min<upx_int8_t>(1, 2) == 1);
static_assert(upx::min<upx_int16_t>(1, 2) == 1);
static_assert(upx::min(1, 2) == 1);
static_assert(upx::min(1ll, 2ll) == 1);
static_assert(upx::max<upx_int8_t>(1, 2) == 2);
static_assert(upx::max<upx_int16_t>(1, 2) == 2);
static_assert(upx::max(1, 2) == 2);
static_assert(upx::max(1ll, 2ll) == 2);
static_assert(upx::wrapping_add<upx_int8_t>(127, 2) == -127);
static_assert(upx::wrapping_add<upx_int16_t>(32767, 2) == -32767);
static_assert(upx::wrapping_add(2147483647, 2) == -2147483647);
static_assert(upx::wrapping_add(9223372036854775807ll, 2ll) == -9223372036854775807ll);
static_assert(upx::wrapping_sub<upx_int8_t>(-127, 2) == 127);
static_assert(upx::wrapping_sub<upx_int16_t>(-32767, 2) == 32767);
static_assert(upx::wrapping_sub(-2147483647, 2) == 2147483647);
static_assert(upx::wrapping_sub(-9223372036854775807ll, 2ll) == 9223372036854775807ll);

namespace compile_time = upx::compile_time;
static_assert(compile_time::string_len("") == 0);
static_assert(compile_time::string_len("a") == 1);
static_assert(compile_time::string_len("ab") == 2);
static_assert(compile_time::string_len("abc") == 3);

static_assert(compile_time::string_eq("", ""));
static_assert(!compile_time::string_eq("a", ""));
static_assert(!compile_time::string_eq("", "a"));
static_assert(compile_time::string_eq("abc", "abc"));
static_assert(!compile_time::string_eq("ab", "abc"));
static_assert(!compile_time::string_eq("abc", "ab"));

static_assert(!compile_time::string_lt("", ""));
static_assert(!compile_time::string_lt("a", ""));
static_assert(compile_time::string_lt("", "a"));
static_assert(!compile_time::string_lt("abc", "abc"));
static_assert(compile_time::string_lt("ab", "abc"));
static_assert(!compile_time::string_lt("abc", "ab"));
static_assert(!compile_time::string_lt("abc", "aba"));
static_assert(compile_time::string_lt("abc", "abz"));

static_assert(compile_time::string_ne("abc", "abz"));
static_assert(!compile_time::string_gt("abc", "abz"));
static_assert(!compile_time::string_ge("abc", "abz"));
static_assert(compile_time::string_le("abc", "abz"));

static_assert(compile_time::mem_eq((const char *) nullptr, (const char *) nullptr, 0));
static_assert(compile_time::mem_eq((const char *) nullptr, (const byte *) nullptr, 0));
static_assert(compile_time::mem_eq((const byte *) nullptr, (const char *) nullptr, 0));
static_assert(compile_time::mem_eq((const byte *) nullptr, (const byte *) nullptr, 0));
static_assert(compile_time::mem_eq("", "", 0));
static_assert(compile_time::mem_eq("abc", "abc", 3));
static_assert(!compile_time::mem_eq("abc", "abz", 3));

static_assert(compile_time::bswap16(0x0102) == 0x0201);
static_assert(compile_time::bswap32(0x01020304) == 0x04030201);
static_assert(compile_time::bswap64(0x0102030405060708ull) == 0x0807060504030201ull);

/*************************************************************************
//
**************************************************************************/

TEST_CASE("std::vector") {
    constexpr size_t N = 16;
    std::vector<int> v(N);
    CHECK(v.end() - v.begin() == N);
    CHECK(&v[0] == &(*(v.begin())));
    // CHECK(&v[0] + N == &(*(v.end()))); // TODO later: is this legal??
#if defined(_LIBCPP_HARDENING_MODE) && defined(_LIBCPP_HARDENING_MODE_DEBUG) &&                    \
    (_LIBCPP_HARDENING_MODE == _LIBCPP_HARDENING_MODE_DEBUG)
    // unfortunately this does not throw but aborts
    ////CHECK_THROWS((void) &v[N]);
#endif
    UNUSED(v);
}

/*************************************************************************
// UPX_CXX_DISABLE_xxx
**************************************************************************/

namespace {
template <class TA, class TB, int TC = 0>
struct MyType1 {
    MyType1() noexcept {}
    UPX_CXX_DISABLE_ADDRESS(MyType1)
    UPX_CXX_DISABLE_COPY_MOVE(MyType1)
    UPX_CXX_DISABLE_NEW_DELETE_NO_VIRTUAL(MyType1)
};
template <class TA, class TB, int TC = 0>
struct MyType2 {
    MyType2() noexcept {}
    UPX_CXX_DISABLE_COPY_MOVE(MyType2)
    typedef MyType2<TA, TB, TC> Self;
    UPX_CXX_DISABLE_ADDRESS(Self)
    UPX_CXX_DISABLE_NEW_DELETE_NO_VIRTUAL(Self)
};
template <class TA, class TB, int TC = 0>
struct MyVType1 {
    MyVType1() noexcept {}
    virtual ~MyVType1() noexcept {}
    UPX_CXX_DISABLE_ADDRESS(MyVType1)
    UPX_CXX_DISABLE_COPY_MOVE(MyVType1)
    UPX_CXX_DISABLE_NEW_DELETE(MyVType1)
};
template <class TA, class TB, int TC = 0>
struct MyVType2 {
    MyVType2() noexcept {}
    virtual ~MyVType2() noexcept {}
    UPX_CXX_DISABLE_COPY_MOVE(MyVType2)
    typedef MyVType2<TA, TB, TC> Self;
    UPX_CXX_DISABLE_ADDRESS(Self)
    UPX_CXX_DISABLE_NEW_DELETE(Self)
};
TEST_CASE("UPX_CXX_DISABLE_xxx") {
    MyType1<char, int, 1> dummy1;
    MyType2<char, int, 2> dummy2;
    MyVType1<char, int, 1> vdummy1;
    MyVType2<char, int, 2> vdummy2;
    (void) dummy1;
    (void) dummy2;
    (void) vdummy1;
    (void) vdummy2;
}
} // namespace

namespace test_disable_new_delete {

struct A1 {
    int a;
};
struct A2 {
    int a;
    UPX_CXX_DISABLE_NEW_DELETE_NO_VIRTUAL(A2)
};
struct B1_A1 : public A1 {
    int b;
};
struct B1_A2 : public A2 {
    int b;
};
struct B2_A1 : public A1 {
    int b;
    UPX_CXX_DISABLE_NEW_DELETE_NO_VIRTUAL(B2_A1)
};
struct B2_A2 : public A2 {
    int b;
    UPX_CXX_DISABLE_NEW_DELETE_NO_VIRTUAL(B2_A2)
};

struct X1 {
    virtual ~X1() noexcept {}
    int x;
};
struct X2 {
    virtual ~X2() noexcept {}
    int x;
    UPX_CXX_DISABLE_NEW_DELETE(X2)
};
struct Y1_X1 : public X1 {
    int y;
};
struct Y1_X2 : public X2 {
    int y;
};
struct Y2_X1 : public X1 {
    int y;
    UPX_CXX_DISABLE_NEW_DELETE(Y2_X1)
};
struct Y2_X2 : public X2 {
    int y;
    UPX_CXX_DISABLE_NEW_DELETE(Y2_X2)
};
struct Z1_X1 : public X1 {
    virtual ~Z1_X1() noexcept {}
    int z;
};
struct Z1_X2 : public X2 {
    virtual ~Z1_X2() noexcept {}
    int z;
};
struct Z2_X1 : public X1 {
    virtual ~Z2_X1() noexcept {}
    int z;
    UPX_CXX_DISABLE_NEW_DELETE(Z2_X1)
};
struct Z2_X2 : public X2 {
    virtual ~Z2_X2() noexcept {}
    int z;
    UPX_CXX_DISABLE_NEW_DELETE(Z2_X2)
};

} // namespace test_disable_new_delete

/*************************************************************************
// util
**************************************************************************/

TEST_CASE("upx::min_max") {
    static_assert(upx::min(0, 0) == 0);
    static_assert(upx::min(0, 1) == 0);
    static_assert(upx::min(1, 0) == 0);
    static_assert(upx::max(0, 0) == 0);
    static_assert(upx::max(0, 1) == 1);
    static_assert(upx::max(1, 0) == 1);
    static_assert(upx::umin(0u, 0u) == 0u);
    static_assert(upx::umin(0u, 1u) == 0u);
    static_assert(upx::umin(1u, 0u) == 0u);
    static_assert(upx::umax(0u, 0u) == 0u);
    static_assert(upx::umax(0u, 1u) == 1u);
    static_assert(upx::umax(1u, 0u) == 1u);
    CHECK_EQ(upx::align_down(0, 4), 0);
    CHECK_EQ(upx::align_down(1, 4), 0);
    CHECK_EQ(upx::align_down(2, 4), 0);
    CHECK_EQ(upx::align_down(3, 4), 0);
    CHECK_EQ(upx::align_down(4, 4), 4);
    CHECK_EQ(upx::align_up(0, 4), 0);
    CHECK_EQ(upx::align_up(1, 4), 4);
    CHECK_EQ(upx::align_up(2, 4), 4);
    CHECK_EQ(upx::align_up(3, 4), 4);
    CHECK_EQ(upx::align_up(4, 4), 4);
    CHECK_EQ(upx::align_gap(0, 4), 0);
    CHECK_EQ(upx::align_gap(1, 4), 3);
    CHECK_EQ(upx::align_gap(2, 4), 2);
    CHECK_EQ(upx::align_gap(3, 4), 1);
    CHECK_EQ(upx::align_gap(4, 4), 0);
}

#if WITH_THREADS
TEST_CASE("upx::ptr_std_atomic_cast") {
    // pointer-size
    CHECK_EQ(upx::ptr_std_atomic_cast((void **) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((uintptr_t *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((upx_uintptr_t *) nullptr), nullptr);
#if 1
    // more fundamental types
    CHECK_EQ(upx::ptr_std_atomic_cast((char *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((short *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((int *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((long *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((ptrdiff_t *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((size_t *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((upx_int8_t *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((upx_int16_t *) nullptr), nullptr);
    CHECK_EQ(upx::ptr_std_atomic_cast((upx_int32_t *) nullptr), nullptr);
#endif
}
#endif

TEST_CASE("upx::atomic_exchange") {
    {
        upx_uintptr_t x = (upx_uintptr_t) 0 - 1;
        upx_uintptr_t y = upx::atomic_exchange(&x, (upx_uintptr_t) 2);
        CHECK_EQ(x, 2);
        CHECK_EQ(y, (upx_uintptr_t) 0 - 1);
        UNUSED(y);
    }
    {
        const int buf[2] = {101, 202};
        const int *ptr_array[2] = {&buf[0], &buf[1]};
        assert_noexcept(*ptr_array[0] == 101 && *ptr_array[1] == 202);
        const int *p = upx::atomic_exchange(&ptr_array[0], ptr_array[1]);
        CHECK_EQ(p, buf + 0);
        assert_noexcept(*ptr_array[0] == 202 && *ptr_array[1] == 202);
        p = upx::atomic_exchange(&ptr_array[1], p);
        CHECK_EQ(p, buf + 1);
        assert_noexcept(*ptr_array[0] == 202 && *ptr_array[1] == 101);
        UNUSED(p);
    }
}

TEST_CASE("upx::ObjectDeleter 1") {
    LE16 *o = nullptr; // object
    LE32 *a = nullptr; // array
    LE64 *m = nullptr; // malloc
    {
        auto o_deleter = upx::ObjectDeleter(&o, 1);
        o = new LE16;
        assert(o != nullptr);
        auto a_deleter = upx::ArrayDeleter(&a, 1);
        a = New(LE32, 1);
        assert(a != nullptr);
        auto m_deleter = upx::MallocDeleter(&m, 1);
        m = (LE64 *) ::malloc(sizeof(LE64));
        assert(m != nullptr);
    }
    assert(o == nullptr);
    assert(a == nullptr);
    assert(m == nullptr);
    // test "const" versions
    {
        const auto o_deleter = upx::ObjectDeleter(&o, 1);
        o = new LE16;
        assert(o != nullptr);
        const auto a_deleter = upx::ArrayDeleter(&a, 1);
        a = New(LE32, 1);
        assert(a != nullptr);
        const auto m_deleter = upx::MallocDeleter(&m, 1);
        m = (LE64 *) ::malloc(sizeof(LE64));
        assert(m != nullptr);
    }
    assert(o == nullptr);
    assert(a == nullptr);
    assert(m == nullptr);
}

TEST_CASE("upx::ObjectDeleter 2") {
    constexpr size_t N = 2;
    BE16 *o[N]; // multiple objects
    BE32 *a[N]; // multiple arrays
    BE64 *m[N]; // multiple mallocs
    {
        auto o_deleter = upx::ObjectDeleter(o, 0);
        auto a_deleter = upx::ArrayDeleter(a, 0);
        auto m_deleter = upx::MallocDeleter(m, 0);
        for (size_t i = 0; i < N; i++) {
            o[i] = new BE16;
            assert(o[i] != nullptr);
            o_deleter.count += 1;
            a[i] = New(BE32, 1 + i);
            assert(a[i] != nullptr);
            a_deleter.count += 1;
            m[i] = (BE64 *) ::malloc(sizeof(BE64));
            assert(m[i] != nullptr);
            m_deleter.count += 1;
        }
    }
    for (size_t i = 0; i < N; i++) {
        assert(o[i] == nullptr);
        assert(a[i] == nullptr);
        assert(m[i] == nullptr);
    }
}

TEST_CASE("upx::ptr_static_cast") {
    // check that we do not trigger any -Wcast-align warnings
    using upx::ptr_static_cast;

    void *vp = nullptr;
    byte *bp = nullptr;
    int *ip = nullptr;
    double *dp = nullptr;

    assert((vp == ptr_static_cast<void *>(vp)));
    assert((vp == ptr_static_cast<void *>(bp)));
    assert((vp == ptr_static_cast<void *>(ip)));
    assert((vp == ptr_static_cast<void *>(dp)));

    assert((bp == ptr_static_cast<byte *>(vp)));
    assert((bp == ptr_static_cast<byte *>(bp)));
    assert((bp == ptr_static_cast<byte *>(ip)));
    assert((bp == ptr_static_cast<byte *>(dp)));

    assert((ip == ptr_static_cast<int *>(vp)));
    assert((ip == ptr_static_cast<int *>(bp)));
    assert((ip == ptr_static_cast<int *>(ip)));
    assert((ip == ptr_static_cast<int *>(dp)));

    assert((dp == ptr_static_cast<double *>(vp)));
    assert((dp == ptr_static_cast<double *>(bp)));
    assert((dp == ptr_static_cast<double *>(ip)));
    assert((dp == ptr_static_cast<double *>(dp)));

    const byte *bc = nullptr;
    const int *ic = nullptr;
    assert((bc == ptr_static_cast<byte *>(bp)));
    assert((bc == ptr_static_cast<const byte *>(bc)));
    assert((bc == ptr_static_cast<byte *>(ip)));
    assert((bc == ptr_static_cast<const byte *>(ic)));
    assert((ic == ptr_static_cast<int *>(bp)));
    assert((ic == ptr_static_cast<const int *>(bc)));
    assert((ic == ptr_static_cast<int *>(ip)));
    assert((ic == ptr_static_cast<const int *>(ic)));
}

TEST_CASE("upx::ptr_static_cast constexpr 1") {
    // check that casts work at compile-time
    using upx::ptr_static_cast;

    constexpr void *vp = nullptr;
    constexpr byte *bp = nullptr;
    constexpr int *ip = nullptr;
    constexpr double *dp = nullptr;
    static_assert((vp == ptr_static_cast<void *>(vp)));
    static_assert((bp == ptr_static_cast<byte *>(bp)));
    static_assert((ip == ptr_static_cast<int *>(ip)));
    static_assert((dp == ptr_static_cast<double *>(dp)));

    constexpr const void *vc = nullptr;
    constexpr const byte *bc = nullptr;
    constexpr const int *ic = nullptr;
    constexpr const double *dc = nullptr;
    static_assert((vc == ptr_static_cast<const void *>(vc)));
    static_assert((bc == ptr_static_cast<const byte *>(bc)));
    static_assert((ic == ptr_static_cast<const int *>(ic)));
    static_assert((dc == ptr_static_cast<const double *>(dc)));

    constexpr void **vpp = nullptr;
    constexpr byte **bpp = nullptr;
    constexpr int **ipp = nullptr;
    constexpr double **dpp = nullptr;
    static_assert((vpp == ptr_static_cast<void **>(vpp)));
    static_assert((bpp == ptr_static_cast<byte **>(bpp)));
    static_assert((ipp == ptr_static_cast<int **>(ipp)));
    static_assert((dpp == ptr_static_cast<double **>(dpp)));

    constexpr const void **vcp = nullptr;
    constexpr const byte **bcp = nullptr;
    constexpr const int **icp = nullptr;
    constexpr const double **dcp = nullptr;
    static_assert((vcp == ptr_static_cast<const void **>(vcp)));
    static_assert((bcp == ptr_static_cast<const byte **>(bcp)));
    static_assert((icp == ptr_static_cast<const int **>(icp)));
    static_assert((dcp == ptr_static_cast<const double **>(dcp)));

    constexpr void *const *vpc = nullptr;
    constexpr byte *const *bpc = nullptr;
    constexpr int *const *ipc = nullptr;
    constexpr double *const *dpc = nullptr;
    static_assert((vpc == ptr_static_cast<void *const *>(vpc)));
    static_assert((bpc == ptr_static_cast<byte *const *>(bpc)));
    static_assert((ipc == ptr_static_cast<int *const *>(ipc)));
    static_assert((dpc == ptr_static_cast<double *const *>(dpc)));

    constexpr const void *const *vcc = nullptr;
    constexpr const byte *const *bcc = nullptr;
    constexpr const int *const *icc = nullptr;
    constexpr const double *const *dcc = nullptr;
    static_assert((vcc == ptr_static_cast<const void *const *>(vcc)));
    static_assert((bcc == ptr_static_cast<const byte *const *>(bcc)));
    static_assert((icc == ptr_static_cast<const int *const *>(icc)));
    static_assert((dcc == ptr_static_cast<const double *const *>(dcc)));
}

TEST_CASE("upx::ptr_static_cast constexpr 2") {
    // check that casts work at compile-time
    using upx::ptr_static_cast;

    constexpr void *vp = nullptr;
    constexpr byte *bp = nullptr;
    constexpr int *ip = nullptr;
    constexpr double *dp = nullptr;
    static_assert((vp == static_cast<void *>(vp)));
    static_assert((vp == static_cast<void *>(bp)));
    static_assert((vp == static_cast<void *>(ip)));
    static_assert((vp == static_cast<void *>(dp)));
    static_assert((vp == ptr_static_cast<void *>(vp)));
    static_assert((vp == ptr_static_cast<void *>(bp)));
    static_assert((vp == ptr_static_cast<void *>(ip)));
    static_assert((vp == ptr_static_cast<void *>(dp)));

    constexpr const void *vc = nullptr;
    constexpr const byte *bc = nullptr;
    constexpr const int *ic = nullptr;
    constexpr const double *dc = nullptr;
    static_assert((vc == static_cast<const void *>(vp)));
    static_assert((vc == static_cast<const void *>(bp)));
    static_assert((vc == static_cast<const void *>(ip)));
    static_assert((vc == static_cast<const void *>(dp)));
    static_assert((vc == ptr_static_cast<const void *>(vp)));
    static_assert((vc == ptr_static_cast<const void *>(dp)));
    static_assert((vc == ptr_static_cast<const void *>(bp)));
    static_assert((vc == ptr_static_cast<const void *>(ip)));
    static_assert((vc == static_cast<const void *>(vc)));
    static_assert((vc == static_cast<const void *>(bc)));
    static_assert((vc == static_cast<const void *>(ic)));
    static_assert((vc == static_cast<const void *>(dc)));
    static_assert((vc == ptr_static_cast<const void *>(vc)));
    static_assert((vc == ptr_static_cast<const void *>(dc)));
    static_assert((vc == ptr_static_cast<const void *>(bc)));
    static_assert((vc == ptr_static_cast<const void *>(ic)));

    // these are invalid:
    //// constexpr char *cc1 = static_cast<char *>(bp);
    //// constexpr char *cc2 = ptr_static_cast<char *>(bp);
    //// constexpr unsigned *uc1 = static_cast<unsigned *>(ip);
    //// constexpr unsigned *uc2 = ptr_static_cast<unsigned *>(ip);
}

TEST_CASE("upx::noncopyable") {
    struct Test : private upx::noncopyable {
        int v = 1;
    };
    Test t = {};
    CHECK(t.v == 1);
#if (ACC_CC_MSC) // MSVC thinks that Test is not std::is_trivially_copyable; true or compiler bug?
    // @COMPILER_BUG @MSVC_BUG
    t.v = 0;
#else
    mem_clear(&t);
#endif
    CHECK(t.v == 0);
    constexpr Test x = {};
    static_assert(x.v == 1);
}

/*************************************************************************
// TriBool checks
**************************************************************************/

namespace {
template <class T>
struct TestTriBool {
    static noinline void test(bool expect_true) {
        static_assert(std::is_class<T>::value);
        static_assert(std::is_nothrow_default_constructible<T>::value);
        static_assert(std::is_nothrow_destructible<T>::value);
        static_assert(std::is_standard_layout<T>::value);
        static_assert(std::is_trivially_copyable<T>::value);
        static_assert(sizeof(typename T::value_type) == sizeof(typename T::underlying_type));
        static_assert(alignof(typename T::value_type) == alignof(typename T::underlying_type));
#if defined(__m68k__) && defined(__atarist__) && defined(__GNUC__)
        // broken compiler or broken ABI
#elif defined(__i386__) && defined(__GNUC__) && (__GNUC__ == 7) && !defined(__clang__)
        static_assert(sizeof(T) == sizeof(typename T::underlying_type));
        // i386: "long long" enum align bug/ABI problem on older compilers
        static_assert(alignof(T) <= alignof(typename T::underlying_type));
#else
        static_assert(sizeof(T) == sizeof(typename T::underlying_type));
        static_assert(alignof(T) == alignof(typename T::underlying_type));
#endif
        static_assert(!bool(T(false)));
        static_assert(bool(T(true)));
        static_assert(bool(T(T::Third)) == T::is_third_true);
        static_assert(T(false) == T::False);
        static_assert(T(true) == T::True);
        static_assert(T(T::False) == T::False);
        static_assert(T(T::True) == T::True);
        static_assert(T(T::Third) == T::Third);
        static_assert(T(T::Third) == T(9));
        static_assert(T(8) == T(9));
        static_assert(!(T(0) == T(9)));
        static_assert(!(T(1) == T(9)));
        static_assert(T(T::Third) == 9);
        static_assert(T(8) == 9);
        static_assert(!(T(0) == 9));
        static_assert(!(T(1) == 9));
        constexpr T array[] = {false, true, T::Third};
        static_assert(array[0].isStrictFalse());
        static_assert(array[1].isStrictTrue());
        static_assert(array[2].isThird());
        static_assert(sizeof(array) == 3 * sizeof(T));
        T a;
        assert(a.getValue() == T::False);
        assert(!a);
        assert(!bool(a));
        assert((!a ? true : false));
        assert(a.isStrictFalse());
        assert(!a.isStrictTrue());
        assert(a.isStrictBool());
        assert(!a.isThird());
        a = false;
        assert(a.getValue() == T::False);
        assert(!a);
        assert(!bool(a));
        assert((!a ? true : false));
        assert(a.isStrictFalse());
        assert(!a.isStrictTrue());
        assert(a.isStrictBool());
        assert(!a.isThird());
        a = true;
        assert(a.getValue() == T::True);
        assert(a);
        assert(bool(a));
        assert((a ? true : false));
        assert(!a.isStrictFalse());
        assert(a.isStrictTrue());
        assert(a.isStrictBool());
        assert(!a.isThird());
        a = T::Third;
        assert(a.getValue() == T::Third);
        assert(T::is_third_true == expect_true);
        if (expect_true) {
            assert(a);
            assert(bool(a));
            assert((a ? true : false));
        } else {
            assert(!a);
            assert(!bool(a));
            assert((!a ? true : false));
        }
        assert(!a.isStrictFalse());
        assert(!a.isStrictTrue());
        assert(!a.isStrictBool());
        assert(a.isThird());
        a = 99;
        assert(a.getValue() == T::Third);
        if (expect_true) {
            assert(a);
            assert(bool(a));
            assert((a ? true : false));
            assert((!a ? false : true));
        } else {
            assert(!a);
            assert(!bool(a));
            assert((a ? false : true));
            assert((!a ? true : false));
        }
        assert(!a.isStrictFalse());
        assert(!a.isStrictTrue());
        assert(!a.isStrictBool());
        assert(a.isThird());
        mem_clear(&a);
        assert(a.isStrictFalse());
    }
};
} // namespace

TEST_CASE("upx::TriBool") {
    using upx::TriBool, upx::tribool;
    static_assert(!tribool(false));
    static_assert(tribool(true));
    static_assert(!tribool(tribool::Third));
    TestTriBool<tribool>::test(false);
#if DEBUG || 1
    TestTriBool<TriBool<upx_int8_t> >::test(false);
    TestTriBool<TriBool<upx_uint8_t> >::test(false);
    TestTriBool<TriBool<upx_int16_t> >::test(false);
    TestTriBool<TriBool<upx_uint16_t> >::test(false);
    TestTriBool<TriBool<upx_int32_t> >::test(false);
    TestTriBool<TriBool<upx_uint32_t> >::test(false);
    TestTriBool<TriBool<upx_int64_t> >::test(false);
    TestTriBool<TriBool<upx_uint64_t> >::test(false);
    TestTriBool<TriBool<upx_int8_t, true> >::test(true);
    TestTriBool<TriBool<upx_uint8_t, true> >::test(true);
    TestTriBool<TriBool<upx_int16_t, true> >::test(true);
    TestTriBool<TriBool<upx_uint16_t, true> >::test(true);
    TestTriBool<TriBool<upx_int32_t, true> >::test(true);
    TestTriBool<TriBool<upx_uint32_t, true> >::test(true);
    TestTriBool<TriBool<upx_int64_t, true> >::test(true);
    TestTriBool<TriBool<upx_uint64_t, true> >::test(true);
#endif
}

/* vim:set ts=4 sw=4 et: */
