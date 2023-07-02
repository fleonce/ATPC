template <typename T> T LL(T *t);

template <typename T> bool SC(T *t, T w);

template <typename T> bool compare_and_swap_weak(T& expected, T desired) {
    T val = LL(expected);
    bool res = SC(expected, desired);
    if (not res) {
        expected = val;
    }
    return res;
}

template <typename T> bool compare_and_swap_strong(T& expected, T desired) {
    T val = LL(expected);
    bool res = SC(expected, desired);
    while(not res and val == expected) {
        val = LL(expected);
        res = SC(expected, desired);
    }
    if (not res) {
        expected = val;
    }
    return res;
}