#ifndef UTILITY_H
#define UTILITY_H

namespace indexes
{
    /** Returns required index of 3D array */
    size_type get3DIdx(const size_type X, const size_type Y,
        const size_type x_i, const size_type y_i, const size_type z_i)
    {
        return z_i * Y * X + y_i * X + x_i;
    }
}

#endif