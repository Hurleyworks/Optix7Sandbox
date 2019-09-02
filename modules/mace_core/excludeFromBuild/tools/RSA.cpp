/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

RSA::RSA()
{
}

RSA::RSA (const juce::String& s)
{
    if (s.containsChar (','))
    {
        part1.parseString (s.upToFirstOccurrenceOf (",", false, false), 16);
        part2.parseString (s.fromFirstOccurrenceOf (",", false, false), 16);
    }
    else
    {
        // the string needs to be two hex numbers, comma-separated..
        jassertfalse;
    }
}

RSA::~RSA()
{
}

bool RSA::operator== (const RSA& other) const noexcept
{
    return part1 == other.part1 && part2 == other.part2;
}

bool RSA::operator!= (const RSA& other) const noexcept
{
    return ! operator== (other);
}

juce::String RSA::toString() const
{
    return part1.toString (16) + "," + part2.toString (16);
}

bool RSA::applyToValue (juce::BigInteger& value) const
{
    if (part1.isZero() || part2.isZero() || value <= 0)
    {
        jassertfalse;   // using an uninitialised key
        value.clear();
        return false;
    }

    juce::BigInteger result;

    while (! value.isZero())
    {
        result *= part2;

        juce::BigInteger remainder;
        value.divideBy (part2, remainder);

        remainder.exponentModulo (part1, part2);

        result += remainder;
    }

    value.swapWith (result);
    return true;
}

juce::BigInteger RSA::findBestCommonDivisor (const juce::BigInteger& p, const juce::BigInteger& q)
{
    // try 3, 5, 9, 17, etc first because these only contain 2 bits and so
    // are fast to divide + multiply
    for (int i = 2; i <= 65536; i *= 2)
    {
        const juce::BigInteger e (1 + i);

        if (e.findGreatestCommonDivisor (p).isOne() && e.findGreatestCommonDivisor (q).isOne())
            return e;
    }

    juce::BigInteger e (4);

    while (! (e.findGreatestCommonDivisor (p).isOne() && e.findGreatestCommonDivisor (q).isOne()))
        ++e;

    return e;
}

void RSA::createKeyPair (RSA& publicKey, RSA& privateKey,
                            const int numBits, const int* randomSeeds, const int numRandomSeeds)
{
    jassert (numBits > 16); // not much point using less than this..
    jassert (numRandomSeeds == 0 || numRandomSeeds >= 2); // you need to provide plenty of seeds here!

    juce::BigInteger p (Prime::createProbablePrime (numBits / 2, 30, randomSeeds, numRandomSeeds / 2));
    juce::BigInteger q (Prime::createProbablePrime (numBits - numBits / 2, 30, randomSeeds == nullptr ? 0 : (randomSeeds + numRandomSeeds / 2), numRandomSeeds - numRandomSeeds / 2));

    const juce::BigInteger n (p * q);
    const juce::BigInteger m (--p * --q);
    const juce::BigInteger e (findBestCommonDivisor (p, q));

    juce::BigInteger d (e);
    d.inverseModulo (m);

    publicKey.part1 = e;
    publicKey.part2 = n;

    privateKey.part1 = d;
    privateKey.part2 = n;
}
