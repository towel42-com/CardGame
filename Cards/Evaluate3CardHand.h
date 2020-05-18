#ifndef __EVALUATE3CARDHAND_H
#define __EVALUATE3CARDHAND_H

// The MIT License( MIT )
//
// Copyright( c ) 2020 Scott Aron Bloom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "CardInfo.h"
#include <memory>
#include <optional>
enum class EHand;
struct SPlayInfo;

namespace NHandUtils
{
    class C3CardInfo : public CCardInfo
    {
    public:
        using THand = std::tuple< TCard, TCard, TCard >;
        C3CardInfo();
        C3CardInfo( const THand& cards );
        C3CardInfo( ECard c1, ESuit s1, ECard c2, ESuit s2, ECard c3, ESuit s3 );
    };

    void generateAll3CardHands();
    uint32_t evaluate3CardHand( const std::vector< std::shared_ptr< CCard > >& cards, const std::shared_ptr< SPlayInfo >& playInfo );
    EHand rankTo3CardHand( uint32_t rank, const std::shared_ptr< SPlayInfo >& playInfo );
}

#endif