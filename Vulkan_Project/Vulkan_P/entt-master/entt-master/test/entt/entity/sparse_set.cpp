#include <memory>
#include <iterator>
#include <exception>
#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <gtest/gtest.h>
#include <entt/core/type_traits.hpp>
#include <entt/entity/sparse_set.hpp>

struct empty_type {};
struct boxed_int { int value; };

TEST(SparseSetNoType, Functionalities) {
    entt::sparse_set<std::uint64_t> set;

    set.reserve(42);

    ASSERT_EQ(set.capacity(), 42);
    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0u);
    ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_EQ(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(42));

    set.construct(42);

    ASSERT_EQ(set.get(42), 0u);

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 1u);
    ASSERT_NE(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_NE(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_TRUE(set.has(42));
    ASSERT_EQ(set.get(42), 0u);

    set.destroy(42);

    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0u);
    ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_EQ(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(42));

    set.construct(42);

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.get(42), 0u);

    ASSERT_TRUE(std::is_move_constructible_v<decltype(set)>);
    ASSERT_TRUE(std::is_move_assignable_v<decltype(set)>);

    entt::sparse_set<std::uint64_t> cpy{set};
    set = cpy;

    ASSERT_FALSE(set.empty());
    ASSERT_FALSE(cpy.empty());
    ASSERT_EQ(set.get(42), 0u);
    ASSERT_EQ(cpy.get(42), 0u);

    entt::sparse_set<std::uint64_t> other{std::move(set)};

    set = std::move(other);
    other = std::move(set);

    ASSERT_TRUE(set.empty());
    ASSERT_FALSE(other.empty());
    ASSERT_EQ(other.get(42), 0u);

    other.reset();

    ASSERT_TRUE(other.empty());
    ASSERT_EQ(other.size(), 0u);
    ASSERT_EQ(std::as_const(other).begin(), std::as_const(other).end());
    ASSERT_EQ(other.begin(), other.end());
    ASSERT_FALSE(other.has(0));
    ASSERT_FALSE(other.has(42));
}

TEST(SparseSetNoType, Pagination) {
    entt::sparse_set<std::uint64_t> set;
    constexpr auto entt_per_page = ENTT_PAGE_SIZE / sizeof(std::uint64_t);

    ASSERT_EQ(set.extent(), 0);

    set.construct(entt_per_page-1);

    ASSERT_EQ(set.extent(), entt_per_page);
    ASSERT_TRUE(set.has(entt_per_page-1));

    set.construct(entt_per_page);

    ASSERT_EQ(set.extent(), 2 * entt_per_page);
    ASSERT_TRUE(set.has(entt_per_page-1));
    ASSERT_TRUE(set.has(entt_per_page));
    ASSERT_FALSE(set.has(entt_per_page+1));

    set.destroy(entt_per_page-1);

    ASSERT_EQ(set.extent(), 2 * entt_per_page);
    ASSERT_FALSE(set.has(entt_per_page-1));
    ASSERT_TRUE(set.has(entt_per_page));

    set.shrink_to_fit();
    set.destroy(entt_per_page);

    ASSERT_EQ(set.extent(), 2 * entt_per_page);
    ASSERT_FALSE(set.has(entt_per_page-1));
    ASSERT_FALSE(set.has(entt_per_page));

    set.shrink_to_fit();

    ASSERT_EQ(set.extent(), 0);
}

TEST(SparseSetNoType, BatchAdd) {
    entt::sparse_set<std::uint64_t> set;
    entt::sparse_set<std::uint64_t>::entity_type entities[2];

    entities[0] = 3;
    entities[1] = 42;

    set.construct(12);
    set.batch(std::begin(entities), std::end(entities));
    set.construct(24);

    ASSERT_TRUE(set.has(entities[0]));
    ASSERT_TRUE(set.has(entities[1]));
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(9));
    ASSERT_TRUE(set.has(12));
    ASSERT_TRUE(set.has(24));

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 4u);
    ASSERT_EQ(set.get(12), 0u);
    ASSERT_EQ(set.get(entities[0]), 1u);
    ASSERT_EQ(set.get(entities[1]), 2u);
    ASSERT_EQ(set.get(24), 3u);
}

TEST(SparseSetNoType, Iterator) {
    using iterator_type = typename entt::sparse_set<std::uint64_t>::iterator_type;

    entt::sparse_set<std::uint64_t> set;
    set.construct(3);

    iterator_type end{set.begin()};
    iterator_type begin{};
    begin = set.end();
    std::swap(begin, end);

    ASSERT_EQ(begin, set.begin());
    ASSERT_EQ(end, set.end());
    ASSERT_NE(begin, end);

    ASSERT_EQ(begin++, set.begin());
    ASSERT_EQ(begin--, set.end());

    ASSERT_EQ(begin+1, set.end());
    ASSERT_EQ(end-1, set.begin());

    ASSERT_EQ(++begin, set.end());
    ASSERT_EQ(--begin, set.begin());

    ASSERT_EQ(begin += 1, set.end());
    ASSERT_EQ(begin -= 1, set.begin());

    ASSERT_EQ(begin + (end - begin), set.end());
    ASSERT_EQ(begin - (begin - end), set.end());

    ASSERT_EQ(end - (end - begin), set.begin());
    ASSERT_EQ(end + (begin - end), set.begin());

    ASSERT_EQ(begin[0], *set.begin());

    ASSERT_LT(begin, end);
    ASSERT_LE(begin, set.begin());

    ASSERT_GT(end, begin);
    ASSERT_GE(end, set.end());

    ASSERT_EQ(*begin, 3);
    ASSERT_EQ(*begin.operator->(), 3);
}

TEST(SparseSetNoType, Find) {
    entt::sparse_set<std::uint64_t> set;
    set.construct(3);
    set.construct(42);
    set.construct(99);

    ASSERT_NE(set.find(3), set.end());
    ASSERT_NE(set.find(42), set.end());
    ASSERT_NE(set.find(99), set.end());
    ASSERT_EQ(set.find(0), set.end());

    auto it = set.find(99);

    ASSERT_EQ(*it, 99);
    ASSERT_EQ(*(++it), 42);
    ASSERT_EQ(*(++it), 3);
    ASSERT_EQ(++it, set.end());
    ASSERT_EQ(++set.find(3), set.end());
}

TEST(SparseSetNoType, Data) {
    entt::sparse_set<std::uint64_t> set;

    set.construct(3);
    set.construct(12);
    set.construct(42);

    ASSERT_EQ(set.get(3), 0u);
    ASSERT_EQ(set.get(12), 1u);
    ASSERT_EQ(set.get(42), 2u);

    ASSERT_EQ(*(set.data() + 0u), 3u);
    ASSERT_EQ(*(set.data() + 1u), 12u);
    ASSERT_EQ(*(set.data() + 2u), 42u);
}

TEST(SparseSetNoType, RespectDisjoint) {
    entt::sparse_set<std::uint64_t> lhs;
    entt::sparse_set<std::uint64_t> rhs;

    lhs.construct(3);
    lhs.construct(12);
    lhs.construct(42);

    ASSERT_EQ(lhs.get(3), 0u);
    ASSERT_EQ(lhs.get(12), 1u);
    ASSERT_EQ(lhs.get(42), 2u);

    lhs.respect(rhs);

    ASSERT_EQ(std::as_const(lhs).get(3), 0u);
    ASSERT_EQ(std::as_const(lhs).get(12), 1u);
    ASSERT_EQ(std::as_const(lhs).get(42), 2u);
}

TEST(SparseSetNoType, RespectOverlap) {
    entt::sparse_set<std::uint64_t> lhs;
    entt::sparse_set<std::uint64_t> rhs;

    lhs.construct(3);
    lhs.construct(12);
    lhs.construct(42);

    rhs.construct(12);

    ASSERT_EQ(lhs.get(3), 0u);
    ASSERT_EQ(lhs.get(12), 1u);
    ASSERT_EQ(lhs.get(42), 2u);

    lhs.respect(rhs);

    ASSERT_EQ(std::as_const(lhs).get(3), 0u);
    ASSERT_EQ(std::as_const(lhs).get(12), 2u);
    ASSERT_EQ(std::as_const(lhs).get(42), 1u);
}

TEST(SparseSetNoType, RespectOrdered) {
    entt::sparse_set<std::uint64_t> lhs;
    entt::sparse_set<std::uint64_t> rhs;

    lhs.construct(1);
    lhs.construct(2);
    lhs.construct(3);
    lhs.construct(4);
    lhs.construct(5);

    ASSERT_EQ(lhs.get(1), 0u);
    ASSERT_EQ(lhs.get(2), 1u);
    ASSERT_EQ(lhs.get(3), 2u);
    ASSERT_EQ(lhs.get(4), 3u);
    ASSERT_EQ(lhs.get(5), 4u);

    rhs.construct(6);
    rhs.construct(1);
    rhs.construct(2);
    rhs.construct(3);
    rhs.construct(4);
    rhs.construct(5);

    ASSERT_EQ(rhs.get(6), 0u);
    ASSERT_EQ(rhs.get(1), 1u);
    ASSERT_EQ(rhs.get(2), 2u);
    ASSERT_EQ(rhs.get(3), 3u);
    ASSERT_EQ(rhs.get(4), 4u);
    ASSERT_EQ(rhs.get(5), 5u);

    rhs.respect(lhs);

    ASSERT_EQ(rhs.get(6), 0u);
    ASSERT_EQ(rhs.get(1), 1u);
    ASSERT_EQ(rhs.get(2), 2u);
    ASSERT_EQ(rhs.get(3), 3u);
    ASSERT_EQ(rhs.get(4), 4u);
    ASSERT_EQ(rhs.get(5), 5u);
}

TEST(SparseSetNoType, RespectReverse) {
    entt::sparse_set<std::uint64_t> lhs;
    entt::sparse_set<std::uint64_t> rhs;

    lhs.construct(1);
    lhs.construct(2);
    lhs.construct(3);
    lhs.construct(4);
    lhs.construct(5);

    ASSERT_EQ(lhs.get(1), 0u);
    ASSERT_EQ(lhs.get(2), 1u);
    ASSERT_EQ(lhs.get(3), 2u);
    ASSERT_EQ(lhs.get(4), 3u);
    ASSERT_EQ(lhs.get(5), 4u);

    rhs.construct(5);
    rhs.construct(4);
    rhs.construct(3);
    rhs.construct(2);
    rhs.construct(1);
    rhs.construct(6);

    ASSERT_EQ(rhs.get(5), 0u);
    ASSERT_EQ(rhs.get(4), 1u);
    ASSERT_EQ(rhs.get(3), 2u);
    ASSERT_EQ(rhs.get(2), 3u);
    ASSERT_EQ(rhs.get(1), 4u);
    ASSERT_EQ(rhs.get(6), 5u);

    rhs.respect(lhs);

    ASSERT_EQ(rhs.get(6), 0u);
    ASSERT_EQ(rhs.get(1), 1u);
    ASSERT_EQ(rhs.get(2), 2u);
    ASSERT_EQ(rhs.get(3), 3u);
    ASSERT_EQ(rhs.get(4), 4u);
    ASSERT_EQ(rhs.get(5), 5u);
}

TEST(SparseSetNoType, RespectUnordered) {
    entt::sparse_set<std::uint64_t> lhs;
    entt::sparse_set<std::uint64_t> rhs;

    lhs.construct(1);
    lhs.construct(2);
    lhs.construct(3);
    lhs.construct(4);
    lhs.construct(5);

    ASSERT_EQ(lhs.get(1), 0u);
    ASSERT_EQ(lhs.get(2), 1u);
    ASSERT_EQ(lhs.get(3), 2u);
    ASSERT_EQ(lhs.get(4), 3u);
    ASSERT_EQ(lhs.get(5), 4u);

    rhs.construct(3);
    rhs.construct(2);
    rhs.construct(6);
    rhs.construct(1);
    rhs.construct(4);
    rhs.construct(5);

    ASSERT_EQ(rhs.get(3), 0u);
    ASSERT_EQ(rhs.get(2), 1u);
    ASSERT_EQ(rhs.get(6), 2u);
    ASSERT_EQ(rhs.get(1), 3u);
    ASSERT_EQ(rhs.get(4), 4u);
    ASSERT_EQ(rhs.get(5), 5u);

    rhs.respect(lhs);

    ASSERT_EQ(rhs.get(6), 0u);
    ASSERT_EQ(rhs.get(1), 1u);
    ASSERT_EQ(rhs.get(2), 2u);
    ASSERT_EQ(rhs.get(3), 3u);
    ASSERT_EQ(rhs.get(4), 4u);
    ASSERT_EQ(rhs.get(5), 5u);
}

TEST(SparseSetNoType, CanModifyDuringIteration) {
    entt::sparse_set<std::uint64_t> set;
    set.construct(0);

    ASSERT_EQ(set.capacity(), entt::sparse_set<std::uint64_t>::size_type{1});

    const auto it = set.begin();
    set.reserve(entt::sparse_set<std::uint64_t>::size_type{2});

    ASSERT_EQ(set.capacity(), entt::sparse_set<std::uint64_t>::size_type{2});

    // this should crash with asan enabled if we break the constraint
    const auto entity = *it;
    (void)entity;
}

TEST(SparseSetWithType, Functionalities) {
    entt::sparse_set<std::uint64_t, int> set;

    set.reserve(42);

    ASSERT_EQ(set.capacity(), 42);
    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0u);
    ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_EQ(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(41));

    set.construct(41, 3);

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 1u);
    ASSERT_NE(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_NE(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_TRUE(set.has(41));
    ASSERT_EQ(set.get(41), 3);
    ASSERT_EQ(*set.try_get(41), 3);
    ASSERT_EQ(set.try_get(99), nullptr);

    set.destroy(41);

    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0u);
    ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_EQ(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(41));

    set.construct(41, 12);

    ASSERT_EQ(set.get(41), 12);
    ASSERT_EQ(*set.try_get(41), 12);
    ASSERT_EQ(set.try_get(99), nullptr);

    set.reset();

    ASSERT_TRUE(set.empty());
    ASSERT_EQ(set.size(), 0u);
    ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
    ASSERT_EQ(set.begin(), set.end());
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(41));

    ASSERT_EQ(set.capacity(), 42);

    set.shrink_to_fit();

    ASSERT_EQ(set.capacity(), 0);

    (void)entt::sparse_set<std::uint64_t, int>{std::move(set)};
    entt::sparse_set<std::uint64_t, int> other;
    other = std::move(set);
}

TEST(SparseSetWithType, EmptyType) {
    entt::sparse_set<std::uint64_t, empty_type> set;

    ASSERT_EQ(&set.construct(42), &set.construct(99));
    ASSERT_EQ(&set.get(42), set.try_get(42));
    ASSERT_EQ(&set.get(42), &set.get(99));
    ASSERT_EQ(std::as_const(set).try_get(42), std::as_const(set).try_get(99));
}

TEST(SparseSetWithType, BatchAdd) {
    entt::sparse_set<std::uint64_t, int> set;
    entt::sparse_set<std::uint64_t>::entity_type entities[2];

    entities[0] = 3;
    entities[1] = 42;

    set.reserve(4);
    set.construct(12, 21);
    auto *component = set.batch(std::begin(entities), std::end(entities));
    set.construct(24, 42);

    ASSERT_TRUE(set.has(entities[0]));
    ASSERT_TRUE(set.has(entities[1]));
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(9));
    ASSERT_TRUE(set.has(12));
    ASSERT_TRUE(set.has(24));

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 4u);
    ASSERT_EQ(set.get(12), 21);
    ASSERT_EQ(set.get(entities[0]), 0);
    ASSERT_EQ(set.get(entities[1]), 0);
    ASSERT_EQ(set.get(24), 42);

    component[0] = 1;
    component[1] = 2;

    ASSERT_EQ(set.get(entities[0]), 1);
    ASSERT_EQ(set.get(entities[1]), 2);
}

TEST(SparseSetWithType, BatchAddEmptyType) {
    entt::sparse_set<std::uint64_t, empty_type> set;
    entt::sparse_set<std::uint64_t>::entity_type entities[2];

    entities[0] = 3;
    entities[1] = 42;

    set.reserve(4);
    set.construct(12);
    auto *component = set.batch(std::begin(entities), std::end(entities));
    set.construct(24);

    ASSERT_TRUE(set.has(entities[0]));
    ASSERT_TRUE(set.has(entities[1]));
    ASSERT_FALSE(set.has(0));
    ASSERT_FALSE(set.has(9));
    ASSERT_TRUE(set.has(12));
    ASSERT_TRUE(set.has(24));

    ASSERT_FALSE(set.empty());
    ASSERT_EQ(set.size(), 4u);
    ASSERT_EQ(&set.get(entities[0]), &set.get(entities[1]));
    ASSERT_EQ(&set.get(entities[0]), &set.get(12));
    ASSERT_EQ(&set.get(entities[0]), &set.get(24));
    ASSERT_EQ(&set.get(entities[0]), component);
}

TEST(SparseSetWithType, AggregatesMustWork) {
    struct aggregate_type { int value; };
    // the goal of this test is to enforce the requirements for aggregate types
    entt::sparse_set<std::uint64_t, aggregate_type>{}.construct(0, 42);
}

TEST(SparseSetWithType, TypesFromStandardTemplateLibraryMustWork) {
    // see #37 - this test shouldn't crash, that's all
    entt::sparse_set<std::uint64_t, std::unordered_set<int>> set;
    set.construct(0).insert(42);
    set.destroy(0);
}

TEST(SparseSetWithType, Iterator) {
    struct internal_type { int value; };

    using iterator_type = typename entt::sparse_set<std::uint64_t, internal_type>::iterator_type;

    entt::sparse_set<std::uint64_t, internal_type> set;
    set.construct(3, 42);

    iterator_type end{set.begin()};
    iterator_type begin{};
    begin = set.end();
    std::swap(begin, end);

    ASSERT_EQ(begin, set.begin());
    ASSERT_EQ(end, set.end());
    ASSERT_NE(begin, end);

    ASSERT_EQ(begin++, set.begin());
    ASSERT_EQ(begin--, set.end());

    ASSERT_EQ(begin+1, set.end());
    ASSERT_EQ(end-1, set.begin());

    ASSERT_EQ(++begin, set.end());
    ASSERT_EQ(--begin, set.begin());

    ASSERT_EQ(begin += 1, set.end());
    ASSERT_EQ(begin -= 1, set.begin());

    ASSERT_EQ(begin + (end - begin), set.end());
    ASSERT_EQ(begin - (begin - end), set.end());

    ASSERT_EQ(end - (end - begin), set.begin());
    ASSERT_EQ(end + (begin - end), set.begin());

    ASSERT_EQ(begin[0].value, set.begin()->value);

    ASSERT_LT(begin, end);
    ASSERT_LE(begin, set.begin());

    ASSERT_GT(end, begin);
    ASSERT_GE(end, set.end());
}

TEST(SparseSetWithType, ConstIterator) {
    struct internal_type { int value; };

    using iterator_type = typename entt::sparse_set<std::uint64_t, internal_type>::const_iterator_type;

    entt::sparse_set<std::uint64_t, internal_type> set;
    set.construct(3, 42);

    iterator_type cend{set.cbegin()};
    iterator_type cbegin{};
    cbegin = set.cend();
    std::swap(cbegin, cend);

    ASSERT_EQ(cbegin, set.cbegin());
    ASSERT_EQ(cend, set.cend());
    ASSERT_NE(cbegin, cend);

    ASSERT_EQ(cbegin++, set.cbegin());
    ASSERT_EQ(cbegin--, set.cend());

    ASSERT_EQ(cbegin+1, set.cend());
    ASSERT_EQ(cend-1, set.cbegin());

    ASSERT_EQ(++cbegin, set.cend());
    ASSERT_EQ(--cbegin, set.cbegin());

    ASSERT_EQ(cbegin += 1, set.cend());
    ASSERT_EQ(cbegin -= 1, set.cbegin());

    ASSERT_EQ(cbegin + (cend - cbegin), set.cend());
    ASSERT_EQ(cbegin - (cbegin - cend), set.cend());

    ASSERT_EQ(cend - (cend - cbegin), set.cbegin());
    ASSERT_EQ(cend + (cbegin - cend), set.cbegin());

    ASSERT_EQ(cbegin[0].value, set.cbegin()->value);

    ASSERT_LT(cbegin, cend);
    ASSERT_LE(cbegin, set.cbegin());

    ASSERT_GT(cend, cbegin);
    ASSERT_GE(cend, set.cend());
}

TEST(SparseSetWithType, IteratorEmptyType) {
    using iterator_type = typename entt::sparse_set<std::uint64_t, empty_type>::iterator_type;
    entt::sparse_set<std::uint64_t, empty_type> set;
    set.construct(3);

    iterator_type end{set.begin()};
    iterator_type begin{};
    begin = set.end();
    std::swap(begin, end);

    ASSERT_EQ(begin, set.begin());
    ASSERT_EQ(end, set.end());
    ASSERT_NE(begin, end);

    ASSERT_EQ(begin++, set.begin());
    ASSERT_EQ(begin--, set.end());

    ASSERT_EQ(begin+1, set.end());
    ASSERT_EQ(end-1, set.begin());

    ASSERT_EQ(++begin, set.end());
    ASSERT_EQ(--begin, set.begin());

    ASSERT_EQ(begin += 1, set.end());
    ASSERT_EQ(begin -= 1, set.begin());

    ASSERT_EQ(begin + (end - begin), set.end());
    ASSERT_EQ(begin - (begin - end), set.end());

    ASSERT_EQ(end - (end - begin), set.begin());
    ASSERT_EQ(end + (begin - end), set.begin());

    ASSERT_EQ(&begin[0], set.begin().operator->());

    ASSERT_LT(begin, end);
    ASSERT_LE(begin, set.begin());

    ASSERT_GT(end, begin);
    ASSERT_GE(end, set.end());

    set.construct(33);

    ASSERT_EQ(&*begin, &*(begin+1));
    ASSERT_EQ(&begin[0], &begin[1]);
    ASSERT_EQ(begin.operator->(), (end-1).operator->());
}

TEST(SparseSetWithType, ConstIteratorEmptyType) {
    using iterator_type = typename entt::sparse_set<std::uint64_t, empty_type>::const_iterator_type;
    entt::sparse_set<std::uint64_t, empty_type> set;
    set.construct(3);

    iterator_type cend{set.cbegin()};
    iterator_type cbegin{};
    cbegin = set.cend();
    std::swap(cbegin, cend);

    ASSERT_EQ(cbegin, set.cbegin());
    ASSERT_EQ(cend, set.cend());
    ASSERT_NE(cbegin, cend);

    ASSERT_EQ(cbegin++, set.cbegin());
    ASSERT_EQ(cbegin--, set.cend());

    ASSERT_EQ(cbegin+1, set.cend());
    ASSERT_EQ(cend-1, set.cbegin());

    ASSERT_EQ(++cbegin, set.cend());
    ASSERT_EQ(--cbegin, set.cbegin());

    ASSERT_EQ(cbegin += 1, set.cend());
    ASSERT_EQ(cbegin -= 1, set.cbegin());

    ASSERT_EQ(cbegin + (cend - cbegin), set.cend());
    ASSERT_EQ(cbegin - (cbegin - cend), set.cend());

    ASSERT_EQ(cend - (cend - cbegin), set.cbegin());
    ASSERT_EQ(cend + (cbegin - cend), set.cbegin());

    ASSERT_EQ(&cbegin[0], set.cbegin().operator->());

    ASSERT_LT(cbegin, cend);
    ASSERT_LE(cbegin, set.cbegin());

    ASSERT_GT(cend, cbegin);
    ASSERT_GE(cend, set.cend());

    set.construct(33);

    ASSERT_EQ(&*cbegin, &*(cbegin+1));
    ASSERT_EQ(&cbegin[0], &cbegin[1]);
    ASSERT_EQ(cbegin.operator->(), (cend-1).operator->());
}

TEST(SparseSetWithType, Raw) {
    entt::sparse_set<std::uint64_t, int> set;

    set.construct(3, 3);
    set.construct(12, 6);
    set.construct(42, 9);

    ASSERT_EQ(set.get(3), 3);
    ASSERT_EQ(std::as_const(set).get(12), 6);
    ASSERT_EQ(set.get(42), 9);

    ASSERT_EQ(*(set.raw() + 0u), 3);
    ASSERT_EQ(*(std::as_const(set).raw() + 1u), 6);
    ASSERT_EQ(*(set.raw() + 2u), 9);
}

TEST(SparseSetWithType, RawEmptyType) {
    entt::sparse_set<std::uint64_t, empty_type> set;

    set.construct(3);

    ASSERT_EQ(set.raw(), std::as_const(set).raw());
    ASSERT_EQ(set.try_get(3), set.raw());
}

TEST(SparseSetWithType, SortOrdered) {
    entt::sparse_set<std::uint64_t, boxed_int> set;

    set.construct(12, boxed_int{12});
    set.construct(42, boxed_int{9});
    set.construct(7, boxed_int{6});
    set.construct(3, boxed_int{3});
    set.construct(9, boxed_int{1});

    ASSERT_EQ(set.get(12).value, 12);
    ASSERT_EQ(set.get(42).value, 9);
    ASSERT_EQ(set.get(7).value, 6);
    ASSERT_EQ(set.get(3).value, 3);
    ASSERT_EQ(set.get(9).value, 1);

    set.sort([](auto lhs, auto rhs) {
        return lhs.value < rhs.value;
    });

    ASSERT_EQ((set.raw() + 0u)->value, 12);
    ASSERT_EQ((set.raw() + 1u)->value, 9);
    ASSERT_EQ((set.raw() + 2u)->value, 6);
    ASSERT_EQ((set.raw() + 3u)->value, 3);
    ASSERT_EQ((set.raw() + 4u)->value, 1);

    auto begin = set.begin();
    auto end = set.end();

    ASSERT_EQ((begin++)->value, 1);
    ASSERT_EQ((begin++)->value, 3);
    ASSERT_EQ((begin++)->value, 6);
    ASSERT_EQ((begin++)->value, 9);
    ASSERT_EQ((begin++)->value, 12);
    ASSERT_EQ(begin, end);
}

TEST(SparseSetWithType, SortReverse) {
    entt::sparse_set<std::uint64_t, boxed_int> set;

    set.construct(12, boxed_int{1});
    set.construct(42, boxed_int{3});
    set.construct(7, boxed_int{6});
    set.construct(3, boxed_int{9});
    set.construct(9, boxed_int{12});

    ASSERT_EQ(set.get(12).value, 1);
    ASSERT_EQ(set.get(42).value, 3);
    ASSERT_EQ(set.get(7).value, 6);
    ASSERT_EQ(set.get(3).value, 9);
    ASSERT_EQ(set.get(9).value, 12);

    set.sort([&set](std::uint64_t lhs, std::uint64_t rhs) {
        return set.get(lhs).value < set.get(rhs).value;
    });

    ASSERT_EQ((set.raw() + 0u)->value, 12);
    ASSERT_EQ((set.raw() + 1u)->value, 9);
    ASSERT_EQ((set.raw() + 2u)->value, 6);
    ASSERT_EQ((set.raw() + 3u)->value, 3);
    ASSERT_EQ((set.raw() + 4u)->value, 1);

    auto begin = set.begin();
    auto end = set.end();

    ASSERT_EQ((begin++)->value, 1);
    ASSERT_EQ((begin++)->value, 3);
    ASSERT_EQ((begin++)->value, 6);
    ASSERT_EQ((begin++)->value, 9);
    ASSERT_EQ((begin++)->value, 12);
    ASSERT_EQ(begin, end);
}

TEST(SparseSetWithType, SortUnordered) {
    entt::sparse_set<std::uint64_t, boxed_int> set;

    set.construct(12, boxed_int{6});
    set.construct(42, boxed_int{3});
    set.construct(7, boxed_int{1});
    set.construct(3, boxed_int{9});
    set.construct(9, boxed_int{12});

    ASSERT_EQ(set.get(12).value, 6);
    ASSERT_EQ(set.get(42).value, 3);
    ASSERT_EQ(set.get(7).value, 1);
    ASSERT_EQ(set.get(3).value, 9);
    ASSERT_EQ(set.get(9).value, 12);

    set.sort([](auto lhs, auto rhs) {
        return lhs.value < rhs.value;
    });

    ASSERT_EQ((set.raw() + 0u)->value, 12);
    ASSERT_EQ((set.raw() + 1u)->value, 9);
    ASSERT_EQ((set.raw() + 2u)->value, 6);
    ASSERT_EQ((set.raw() + 3u)->value, 3);
    ASSERT_EQ((set.raw() + 4u)->value, 1);

    auto begin = set.begin();
    auto end = set.end();

    ASSERT_EQ((begin++)->value, 1);
    ASSERT_EQ((begin++)->value, 3);
    ASSERT_EQ((begin++)->value, 6);
    ASSERT_EQ((begin++)->value, 9);
    ASSERT_EQ((begin++)->value, 12);
    ASSERT_EQ(begin, end);
}

TEST(SparseSetWithType, RespectDisjoint) {
    entt::sparse_set<std::uint64_t, int> lhs;
    entt::sparse_set<std::uint64_t, int> rhs;

    lhs.construct(3, 3);
    lhs.construct(12, 6);
    lhs.construct(42, 9);

    ASSERT_EQ(std::as_const(lhs).get(3), 3);
    ASSERT_EQ(std::as_const(lhs).get(12), 6);
    ASSERT_EQ(std::as_const(lhs).get(42), 9);

    lhs.respect(rhs);

    ASSERT_EQ(*(std::as_const(lhs).raw() + 0u), 3);
    ASSERT_EQ(*(std::as_const(lhs).raw() + 1u), 6);
    ASSERT_EQ(*(std::as_const(lhs).raw() + 2u), 9);

    auto begin = lhs.begin();
    auto end = lhs.end();

    ASSERT_EQ(*(begin++), 9);
    ASSERT_EQ(*(begin++), 6);
    ASSERT_EQ(*(begin++), 3);
    ASSERT_EQ(begin, end);
}

TEST(SparseSetWithType, RespectOverlap) {
    entt::sparse_set<std::uint64_t, int> lhs;
    entt::sparse_set<std::uint64_t, int> rhs;

    lhs.construct(3, 3);
    lhs.construct(12, 6);
    lhs.construct(42, 9);
    rhs.construct(12, 6);

    ASSERT_EQ(std::as_const(lhs).get(3), 3);
    ASSERT_EQ(std::as_const(lhs).get(12), 6);
    ASSERT_EQ(std::as_const(lhs).get(42), 9);
    ASSERT_EQ(rhs.get(12), 6);

    lhs.respect(rhs);

    ASSERT_EQ(*(std::as_const(lhs).raw() + 0u), 3);
    ASSERT_EQ(*(std::as_const(lhs).raw() + 1u), 9);
    ASSERT_EQ(*(std::as_const(lhs).raw() + 2u), 6);

    auto begin = lhs.begin();
    auto end = lhs.end();

    ASSERT_EQ(*(begin++), 6);
    ASSERT_EQ(*(begin++), 9);
    ASSERT_EQ(*(begin++), 3);
    ASSERT_EQ(begin, end);
}

TEST(SparseSetWithType, RespectOrdered) {
    entt::sparse_set<std::uint64_t, int> lhs;
    entt::sparse_set<std::uint64_t, int> rhs;

    lhs.construct(1, 0);
    lhs.construct(2, 0);
    lhs.construct(3, 0);
    lhs.construct(4, 0);
    lhs.construct(5, 0);

    ASSERT_EQ(lhs.get(1), 0);
    ASSERT_EQ(lhs.get(2), 0);
    ASSERT_EQ(lhs.get(3), 0);
    ASSERT_EQ(lhs.get(4), 0);
    ASSERT_EQ(lhs.get(5), 0);

    rhs.construct(6, 0);
    rhs.construct(1, 0);
    rhs.construct(2, 0);
    rhs.construct(3, 0);
    rhs.construct(4, 0);
    rhs.construct(5, 0);

    ASSERT_EQ(rhs.get(6), 0);
    ASSERT_EQ(rhs.get(1), 0);
    ASSERT_EQ(rhs.get(2), 0);
    ASSERT_EQ(rhs.get(3), 0);
    ASSERT_EQ(rhs.get(4), 0);
    ASSERT_EQ(rhs.get(5), 0);

    rhs.respect(lhs);

    ASSERT_EQ(*(lhs.data() + 0u), 1u);
    ASSERT_EQ(*(lhs.data() + 1u), 2u);
    ASSERT_EQ(*(lhs.data() + 2u), 3u);
    ASSERT_EQ(*(lhs.data() + 3u), 4u);
    ASSERT_EQ(*(lhs.data() + 4u), 5u);

    ASSERT_EQ(*(rhs.data() + 0u), 6u);
    ASSERT_EQ(*(rhs.data() + 1u), 1u);
    ASSERT_EQ(*(rhs.data() + 2u), 2u);
    ASSERT_EQ(*(rhs.data() + 3u), 3u);
    ASSERT_EQ(*(rhs.data() + 4u), 4u);
    ASSERT_EQ(*(rhs.data() + 5u), 5u);
}

TEST(SparseSetWithType, RespectReverse) {
    entt::sparse_set<std::uint64_t, int> lhs;
    entt::sparse_set<std::uint64_t, int> rhs;

    lhs.construct(1, 0);
    lhs.construct(2, 0);
    lhs.construct(3, 0);
    lhs.construct(4, 0);
    lhs.construct(5, 0);

    ASSERT_EQ(lhs.get(1), 0);
    ASSERT_EQ(lhs.get(2), 0);
    ASSERT_EQ(lhs.get(3), 0);
    ASSERT_EQ(lhs.get(4), 0);
    ASSERT_EQ(lhs.get(5), 0);

    rhs.construct(5, 0);
    rhs.construct(4, 0);
    rhs.construct(3, 0);
    rhs.construct(2, 0);
    rhs.construct(1, 0);
    rhs.construct(6, 0);

    ASSERT_EQ(rhs.get(5), 0);
    ASSERT_EQ(rhs.get(4), 0);
    ASSERT_EQ(rhs.get(3), 0);
    ASSERT_EQ(rhs.get(2), 0);
    ASSERT_EQ(rhs.get(1), 0);
    ASSERT_EQ(rhs.get(6), 0);

    rhs.respect(lhs);

    ASSERT_EQ(*(lhs.data() + 0u), 1u);
    ASSERT_EQ(*(lhs.data() + 1u), 2u);
    ASSERT_EQ(*(lhs.data() + 2u), 3u);
    ASSERT_EQ(*(lhs.data() + 3u), 4u);
    ASSERT_EQ(*(lhs.data() + 4u), 5u);

    ASSERT_EQ(*(rhs.data() + 0u), 6u);
    ASSERT_EQ(*(rhs.data() + 1u), 1u);
    ASSERT_EQ(*(rhs.data() + 2u), 2u);
    ASSERT_EQ(*(rhs.data() + 3u), 3u);
    ASSERT_EQ(*(rhs.data() + 4u), 4u);
    ASSERT_EQ(*(rhs.data() + 5u), 5u);
}

TEST(SparseSetWithType, RespectUnordered) {
    entt::sparse_set<std::uint64_t, int> lhs;
    entt::sparse_set<std::uint64_t, int> rhs;

    lhs.construct(1, 0);
    lhs.construct(2, 0);
    lhs.construct(3, 0);
    lhs.construct(4, 0);
    lhs.construct(5, 0);

    ASSERT_EQ(lhs.get(1), 0);
    ASSERT_EQ(lhs.get(2), 0);
    ASSERT_EQ(lhs.get(3), 0);
    ASSERT_EQ(lhs.get(4), 0);
    ASSERT_EQ(lhs.get(5), 0);

    rhs.construct(3, 0);
    rhs.construct(2, 0);
    rhs.construct(6, 0);
    rhs.construct(1, 0);
    rhs.construct(4, 0);
    rhs.construct(5, 0);

    ASSERT_EQ(rhs.get(3), 0);
    ASSERT_EQ(rhs.get(2), 0);
    ASSERT_EQ(rhs.get(6), 0);
    ASSERT_EQ(rhs.get(1), 0);
    ASSERT_EQ(rhs.get(4), 0);
    ASSERT_EQ(rhs.get(5), 0);

    rhs.respect(lhs);

    ASSERT_EQ(*(lhs.data() + 0u), 1u);
    ASSERT_EQ(*(lhs.data() + 1u), 2u);
    ASSERT_EQ(*(lhs.data() + 2u), 3u);
    ASSERT_EQ(*(lhs.data() + 3u), 4u);
    ASSERT_EQ(*(lhs.data() + 4u), 5u);

    ASSERT_EQ(*(rhs.data() + 0u), 6u);
    ASSERT_EQ(*(rhs.data() + 1u), 1u);
    ASSERT_EQ(*(rhs.data() + 2u), 2u);
    ASSERT_EQ(*(rhs.data() + 3u), 3u);
    ASSERT_EQ(*(rhs.data() + 4u), 4u);
    ASSERT_EQ(*(rhs.data() + 5u), 5u);
}

TEST(SparseSetWithType, RespectOverlapEmptyType) {
    entt::sparse_set<std::uint64_t, empty_type> lhs;
    entt::sparse_set<std::uint64_t, empty_type> rhs;

    lhs.construct(3);
    lhs.construct(12);
    lhs.construct(42);

    rhs.construct(12);

    ASSERT_EQ(lhs.sparse_set<std::uint64_t>::get(3), 0u);
    ASSERT_EQ(lhs.sparse_set<std::uint64_t>::get(12), 1u);
    ASSERT_EQ(lhs.sparse_set<std::uint64_t>::get(42), 2u);

    lhs.respect(rhs);

    ASSERT_EQ(std::as_const(lhs).sparse_set<std::uint64_t>::get(3), 0u);
    ASSERT_EQ(std::as_const(lhs).sparse_set<std::uint64_t>::get(12), 2u);
    ASSERT_EQ(std::as_const(lhs).sparse_set<std::uint64_t>::get(42), 1u);
}

TEST(SparseSetWithType, CanModifyDuringIteration) {
    entt::sparse_set<std::uint64_t, int> set;
    set.construct(0, 42);

    ASSERT_EQ(set.capacity(), entt::sparse_set<std::uint64_t>::size_type{1});

    const auto it = set.cbegin();
    set.reserve(entt::sparse_set<std::uint64_t>::size_type{2});

    ASSERT_EQ(set.capacity(), entt::sparse_set<std::uint64_t>::size_type{2});

    // this should crash with asan enabled if we break the constraint
    const auto entity = *it;
    (void)entity;
}

TEST(SparseSetWithType, ReferencesGuaranteed) {
    struct internal_type { int value; };

    entt::sparse_set<std::uint64_t, internal_type> set;

    set.construct(0, 0);
    set.construct(1, 1);

    ASSERT_EQ(set.get(0).value, 0);
    ASSERT_EQ(set.get(1).value, 1);

    for(auto &&type: set) {
        if(type.value) {
            type.value = 42;
        }
    }

    ASSERT_EQ(set.get(0).value, 0);
    ASSERT_EQ(set.get(1).value, 42);

    auto begin = set.begin();

    while(begin != set.end()) {
        (begin++)->value = 3;
    }

    ASSERT_EQ(set.get(0).value, 3);
    ASSERT_EQ(set.get(1).value, 3);
}

TEST(SparseSetWithType, MoveOnlyComponent) {
    // the purpose is to ensure that move only components are always accepted
    entt::sparse_set<std::uint64_t, std::unique_ptr<int>> set;
    (void)set;
}

TEST(SparseSetWithType, ConstructorExceptionDoesNotAddToSet) {
    struct throwing_component {
        struct constructor_exception: std::exception {};

        [[noreturn]] throwing_component() { throw constructor_exception{}; }

        // necessary to avoid the short-circuit construct() logic for empty objects
        int data;
    };

    entt::sparse_set<std::uint64_t, throwing_component> set;

    try {
        set.construct(0);
        FAIL() << "Expected constructor_exception to be thrown";
    } catch (const throwing_component::constructor_exception &) {
        ASSERT_TRUE(set.empty());
    }
}
