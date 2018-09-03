//
// Created by koushiro on 9/3/18.
//

#include "tento/base/Duration.hpp"
using namespace tento;

int main() {
    Duration duration = Duration(0, 0);
    assert(duration.IsZero());

    duration = Duration::FromNanos(12'345'000'000);
    assert(duration.SecsPart() == 12);
    assert(duration.SubSecNanos() == 345'000'000);
    assert(duration.SubSecMicros() == 345'000);
    assert(duration.SubSecMillis() == 345);
    assert(duration.AsNanos() == 12'345'000'000);
    assert(duration.AsMicros() == 12'345'000);
    assert(duration.AsMillis() == 12'345);
    assert(duration.AsSecs() == 12.345);

    duration = Duration::FromMicros(12'345'000);
    assert(duration.SecsPart() == 12);
    assert(duration.SubSecMicros() == 345'000);

    duration = Duration::FromMillis(12'345);
    assert(duration.SecsPart() == 12);
    assert(duration.SubSecMillis() == 345);

    duration = Duration::FromSecs(12'345);
    assert(duration.SecsPart() == 12'345);
    assert(duration.SubSecNanos() == 0);
    assert(duration.AsMins() == static_cast<double>(12'345) / 60);
    assert(duration.AsHours() == static_cast<double>(12'345) / 3600);

    duration = Duration::FromMins(30);
    assert(duration.SecsPart() == 30*60);
    assert(duration.SubSecNanos() == 0);

    duration = Duration::FromHours(12);
    assert(duration.SecsPart() == 12*3600);
    assert(duration.SubSecNanos() == 0);

    Duration d1 = Duration::FromNanos(12'345'000'000);
    Duration d2 = Duration::FromNanos( 1'234'500'000);
    assert(d1 > d2);
    assert(d1 != d2);
    assert(d1 == Duration::FromNanos(12'345'000'000));
    assert((d1 + d2) == Duration::FromNanos(13'579'500'000));
    assert((d1 - d2) == Duration::FromNanos(11'110'500'000));
    assert(d1 * 2 == Duration::FromNanos(24'690'000'000));
    assert(d1 / 2 == Duration::FromNanos(6'172'500'000));

    return 0;
}