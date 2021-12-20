#ifndef G_HPP
#define G_HPP

class Gadget
{
    inline static int gen_id = 0;

    int id_ = ++gen_id;
public:
    Gadget() = default;

    int id() const
    {
        return id_;
    }
};

#endif