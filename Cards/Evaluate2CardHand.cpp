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

#include "Evaluate2CardHand.h"
#include "Hand.h"
#include "PlayInfo.h"
#include "HandUtils.h"

#include <map>
#include <set>
#include <optional>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>

namespace NHandUtils
{
    bool C2CardInfo::sAllHandsComputed{ false };

    C2CardInfo::C2CardInfo() :
        C2CardInfo( THand( TCard( ECard::eUNKNOWN, ESuit::eUNKNOWN ), TCard( ECard::eUNKNOWN, ESuit::eUNKNOWN ) ) )
    {

    }

    C2CardInfo::C2CardInfo( ECard c1, ESuit s1, ECard c2, ESuit s2 ) :
        C2CardInfo( THand( TCard( c1, s1 ), TCard( c2, s2 ) ) )
    {

    }

    C2CardInfo::C2CardInfo( const THand& cards )
    {
        fHandOrder = { EHand::eStraightFlush, EHand::ePair, EHand::eStraight, EHand::eFlush, EHand::eHighCard };

        setOrigCards( { std::get< 0 >( cards ), std::get< 1 >( cards ) } );
           
        fIsFlush = NHandUtils::isFlush( fOrigCards );
        fIsPair = NHandUtils::isCount( fOrigCards, 2 );
        fStraightType = NHandUtils::isStraight( fOrigCards );
    }

    void C2CardInfo::generateAll2CardHands()
    {
        if ( NHandUtils::gComputeAllHands && !sAllHandsComputed )
        {
            sAllHandsComputed = true;

            using TCardToInfoMap = std::map< C2CardInfo::THand, C2CardInfo >;
            auto gFlushStraightsCount = []( const C2CardInfo& lhs, const C2CardInfo& rhs ) { return lhs.greaterThan( true, rhs ); };
            auto gJustCardsCount = []( const C2CardInfo& lhs, const C2CardInfo& rhs ) { return lhs.greaterThan( false, rhs ); };

            using TCardsCountMap = std::map< C2CardInfo, uint32_t, decltype( gJustCardsCount ) >;
            using TFlushesCountMap = std::map< C2CardInfo, uint32_t, decltype( gFlushStraightsCount ) >;

            static TCardToInfoMap allHands;
            static TCardsCountMap justCardsCount( gJustCardsCount );
            static TFlushesCountMap flushesAndStraightsCount( gFlushStraightsCount );

            auto numHands = 52 * 51;
            std::cout << "Generating: " << numHands << "\n";

            size_t maxCardsValue = 0;

            auto&& allCards = CCard::allCardsList();
            uint64_t handCount = 0;

            for ( auto&& c1 : allCards )
            {
                for ( auto&& c2 : allCards )
                {
                    if ( *c1 == *c2 )
                        continue;

                    handCount++;
                    if ( ( handCount % ( numHands / 10 ) ) == 0 )
                        std::cout << "   Generating: Hand #" << handCount << " of " << numHands << "\n";

                    auto curr = C2CardInfo::THand( TCard( c1->getCard(), c1->getSuit() ), TCard( c2->getCard(), c2->getSuit() ) );
                    C2CardInfo cardInfo( curr );
                    allHands[ curr ] = cardInfo;
                    justCardsCount.insert( std::make_pair( cardInfo, -1 ) );
                    flushesAndStraightsCount.insert( std::make_pair( cardInfo, -1 ) );

                    maxCardsValue = std::max( static_cast<size_t>( cardInfo.getCardsValue() ), maxCardsValue );
                }
            }

            std::cout << "Finished Generating: " << numHands << "\n";

            std::ofstream ofs( "E:/DropBox/Documents/sb/github/scottaronbloom/CardGame/Cards/2CardDump.cpp" );
            std::ostream* oss = &ofs; //&std::cout;

            CCardInfo::computeAndGenerateMaps( *oss, 2, justCardsCount, flushesAndStraightsCount );

            std::vector< uint32_t > flushes;
            flushes.resize( maxCardsValue + 1 );

            std::vector< uint32_t > highCardUnique;
            highCardUnique.resize( maxCardsValue + 1 );

            std::vector< uint32_t > straightsUnique;
            straightsUnique.resize( maxCardsValue + 1 );

            std::map< uint64_t, uint16_t > highCardProductMap;
            std::map< uint64_t, uint16_t > straightsProductMap;

            for ( auto&& ii : allHands )
            {
                auto cardValue = ii.second.getCardsValue();

                auto pos = flushesAndStraightsCount.find( ii.second );
                Q_ASSERT( pos != flushesAndStraightsCount.end() );
                auto straightsValue = ( *pos ).second;

                auto pos2 = justCardsCount.find( ii.second );
                Q_ASSERT( pos2 != justCardsCount.end() );
                auto highCardValue = ( *pos2 ).second;

                if ( ii.second.isFlush() )
                {
                    flushes[ cardValue ] = straightsValue;
                }
                else if ( ii.second.allCardsUnique() )
                {
                    straightsUnique[ cardValue ] = straightsValue;
                    highCardUnique[ cardValue ] = highCardValue;
                }
                else
                {
                    auto productValue = ii.second.handProduct();
                    straightsProductMap[ productValue ] = straightsValue;
                    highCardProductMap[ productValue ] = highCardValue;
                }
            }
            CCardInfo::generateTable( *oss, flushes, "sFlushes" );
            CCardInfo::generateTable( *oss, highCardUnique, "sHighCardUnique" );
            CCardInfo::generateTable( *oss, straightsUnique, "sStraightsUnique" );
            CCardInfo::generateMap( *oss, highCardProductMap, "sProductMap" );
            CCardInfo::generateMap( *oss, straightsProductMap, "sStraitsAndFlushesProductMap" );

            CCardInfo::generateEvaluateFunction( *oss, 2 );
            CCardInfo::generateRankFunction( *oss, 2, justCardsCount, flushesAndStraightsCount );
        }
    }
}
