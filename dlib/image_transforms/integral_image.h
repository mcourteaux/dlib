// Copyright (C) 2009  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_INTEGRAL_IMAGE
#define DLIB_INTEGRAL_IMAGE

#include "integral_image_abstract.h"

#include "../algs.h"
#include "../assert.h"
#include "../geometry.h"
#include "../array2d.h"
#include "../matrix.h"
#include "../pixel.h"
#include "../noncopyable.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class integral_image;
    inline const rectangle get_rect (
        const integral_image& img
    );

// ----------------------------------------------------------------------------------------

    class integral_image : noncopyable
    {
    public:
        typedef long value_type;

        const long nr() const { return int_img.nr(); }
        const long nc() const { return int_img.nc(); }

        template <typename image_type>
        void load (
            const image_type& img
        )
        {
            unsigned long pixel;
            int_img.set_size(img.nr(), img.nc());

            // compute the first row of the integral image
            unsigned long temp = 0;
            for (unsigned long c = 0; c < img.nc(); ++c)
            {
                assign_pixel(pixel, img[0][c]);
                temp += pixel;
                int_img[0][c] = temp;
            }

            // now compute the rest of the integral image
            for (unsigned long r = 1; r < img.nr(); ++r)
            {
                temp = 0;
                for (unsigned long c = 0; c < img.nc(); ++c)
                {
                    assign_pixel(pixel, img[r][c]);
                    temp += pixel;
                    int_img[r][c] = temp + int_img[r-1][c];
                }
            }

        }

        long get_sum_of_area (
            const rectangle& rect
        ) const
        {
            DLIB_ASSERT(get_rect(*this).contains(rect) == true,
                "\tlong get_sum_of_area(rect)"
                << "\n\tYou have given a rectangle that goes outside the image"
                << "\n\tthis:            " << this
                << "\n\trect:            " << rect 
                << "\n\tget_rect(*this): " << get_rect(*this) 
            );

            unsigned long top_left = 0, top_right = 0, bottom_left = 0, bottom_right = 0;

            bottom_right = int_img[rect.bottom()][rect.right()];
            if (rect.left()-1 >= 0 && rect.top()-1 >= 0)
            {
                top_left = int_img[rect.top()-1][rect.left()-1];
                bottom_left = int_img[rect.bottom()][rect.left()-1];
                top_right = int_img[rect.top()-1][rect.right()];
            }
            else if (rect.left()-1 >= 0)
            {
                bottom_left = int_img[rect.bottom()][rect.left()-1];
            }
            else if (rect.top()-1 >= 0)
            {
                top_right = int_img[rect.top()-1][rect.right()];
            }

            return bottom_right - bottom_left - top_right + top_left;
        }

    private:

        array2d<unsigned long>::kernel_1a int_img;


    };

// ----------------------------------------------------------------------------------------

    inline const rectangle get_rect (
        const integral_image& img
    )
    {
        return rectangle(0, 0, img.nc()-1, img.nr()-1);
    }

    //  ----------------------------------------------------------------------------

    template <typename integral_image_type>
    typename integral_image_type::value_type haar_x (
        const integral_image_type& img,
        const point& p,
        long width
    )
    {
        DLIB_ASSERT(get_rect(img).contains(centered_rect(p,width,width)) == true,
            "\tlong haar_x(img,p,width)"
            << "\n\tYou have given a point and with that goes outside the image"
            << "\n\tget_rect(img):  " << get_rect(img) 
            << "\n\tp:              " << p 
            << "\n\twidth:          " << width 
        );

        rectangle left_rect;
        left_rect.set_left ( p.x() - width / 2 );
        left_rect.set_top ( p.y() - width / 2 );
        left_rect.set_right ( p.x()-1 );
        left_rect.set_bottom ( left_rect.top() + width - 1 );

        rectangle right_rect;
        right_rect.set_left ( p.x() );
        right_rect.set_top ( left_rect.top() );
        right_rect.set_right ( left_rect.left() + width -1 );
        right_rect.set_bottom ( left_rect.bottom() );

        return img.get_sum_of_area(right_rect) - img.get_sum_of_area(left_rect);
    }

    //  ----------------------------------------------------------------------------

    template <typename integral_image_type>
    typename integral_image_type::value_type haar_y (
        const integral_image_type& img,
        const point& p,
        long width
    )
    {
        DLIB_ASSERT(get_rect(img).contains(centered_rect(p,width,width)) == true,
            "\tlong haar_y(img,p,width)"
            << "\n\tYou have given a point and with that goes outside the image"
            << "\n\tget_rect(img):  " << get_rect(img) 
            << "\n\tp:              " << p 
            << "\n\twidth:          " << width 
        );

        rectangle top_rect;
        top_rect.set_left ( p.x() - width / 2 );
        top_rect.set_top ( p.y() - width / 2 );
        top_rect.set_right ( top_rect.left() + width - 1 );
        top_rect.set_bottom ( p.y()-1 );

        rectangle bottom_rect;
        bottom_rect.set_left ( top_rect.left() );
        bottom_rect.set_top ( p.y() );
        bottom_rect.set_right ( top_rect.right() );
        bottom_rect.set_bottom ( top_rect.top() + width - 1 );

        return img.get_sum_of_area(bottom_rect) - img.get_sum_of_area(top_rect);
    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_INTEGRAL_IMAGE

