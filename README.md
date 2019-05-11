# Emperor's Clothes

A DirectShow filter that hides idle cursor.

## Install

Because this filter will simply pass-through all video data, it is preferred to put it in the filter chain as early as possible to avoid being dropped by other filters, while not affecting any filter.

## Build

Dependencies:
    https://github.com/microsoft/Windows-classic-samples
