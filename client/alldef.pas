{ Alldefines - muss im Moment noch included werden ... }

{$ifndef __ALLDEF_PAS__}
{$define __ALLDEF_PAS__}

{$define SECS_TO_TIMER(x)      (longint((x) * TIMERS_PER_SECOND))}
{$define MSEC_TO_TIMER(x)      (longint((x) * (TIMERS_PER_SECOND DIV 1000)))}
{$define BPS_TO_TIMER(x)       (TIMERS_PER_SECOND DIV longint(x)) }
{$define BPM_TO_TIMER(x)       ((60 * TIMERS_PER_SECOND) DIV longint((x)))}

{$endif}

