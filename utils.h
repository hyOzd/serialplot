
// credits: peppe@stackoverflow [http://stackoverflow.com/a/16795664/432492]
template<typename... Args> struct SELECT {
    template<typename C, typename R>
    static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};
