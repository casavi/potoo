# potoo

potoo is a simple command line tool which takes a pdf file and performs ocr on one or multiple regions per page.  
Highly threaded and performant, with no temporary files being created.  
Ideal for usage in automated systems, e.g. NodeJS or Python scripts.  

Homepage: [casavi.de](http://casavi.de/)  
Author: [Markus Heimbuchner](https://github.com/MHeimbuc)  
Contact us: <hello@casavi.de>

### Parameters
```sh
# ./potoo --help
Usage:
potoo -c <config-file> <output-command: -S|-h|-o|-i> [<range-options>]

Allowed options:
  --help                        produce this help message
  -c [--config] path            path to config json file (Required)

And ONE of the following:
  -S [--single_page] path       render the nth page (--page) of the pdf as PNG into <path>
  -h [--human]                  process everything and print to stdout, mainly for debugging
  -o [--output] path            process everything and save it as json at <path>
  -i [--info] page              provide information about the pdf file and save it at <path>
Additional options:
  Range options:
    -s [--start] start          The page on which to start (0-based). 0 if not specified
    -e [--end] end              The last page to process (0-based). <page-count> if not specified
  Position options:
    -p [--page] page            Only work on a single page

Examples:
  Render the first page into first_page.png
    potoo -c file.json -S first_page.png -p 0
  Extract text from the first 3 pages and display it in human readable form
    potoo -c file.json -h -e 3
  Extract text from the 9th page (0-based) and display it in human readable form
    potoo -c file.json -h -p 8
  Get the page count of a pdf and save it into pagecount.json
    potoo -c file.json -i pagecount.json
```

### Config file syntax
Valid JSON file in the following format:
- inputPDF: String, absolute/relative path to input file
- dpi: Int, the DPI to render the PDF with (in PPI)
- language: String, [Tesseract language name](https://github.com/tesseract-ocr/tessdata) - install your wanted packages
- parallel_processing: Bool, if multiple paged pdfs should be processed in parallel or not. Optional, default is single threaded
- crops: Array, multiple regions PER PAGE to OCR. Optional (only if it is not needed!), may be empty or not present at all.
    - type: String, unique name to identify this region later
    - x: Float, percentage distance from the left of the page
    - y: Float, percentage distance from the top
    - w: Float, percentage width of the region
    - h: Float, percentage height of the region

### Options
##### Actions/Output
- --help: Print usage and helpful information
- -o [--output] <path>: Performs OCR or text extraction on all crop regions of all* pages of the PDF, saves all results in the file specified via <path>. 
- -S [--single_page] <path>: Renders one page of the document into the specified PNG file. Requires the --page parameter to specify which page to render.
- -h [--human]: Performs the same things as -o but prints it in very simple human readable format. Mainly for testing.
- -i [--info] <path>: Prints infos about the PDF into <path>, in JSON format. Format: ```{ "pages": "<count>" }```

> \* can be adjusted by specifying either -s, -e or -p

##### Modifiers
- -s <start>: Page where to start
- -e <end>: Page where to end (still included)
- -p <page>: Only this specific page (mainly useful for --single_page)


### Hints for Usage
Use the --single_page parameter to get the first page as picture exactly as the tool would if it's run normally.  
Then calculate your region(s) based off this picture and run it with --human for testing.  
If everything went right you should get your text and you can use it via automation.

### Example Usage and Output
[Example PDF](http://solutions.weblite.ca/pdfocrx/scansmpl.pdf)  
/tmp/potoo_example.json
```json
{
  "inputPDF": "/tmp/potoo_example.pdf",
  "dpi": 250,
  "language": "eng",
  "parallel_processing": true,
  "crops": [
    {
      "type": "address",
      "dimensions": {
        "x": 16,
        "y": 29,
        "w": 20,
        "h": 10
      }
    }
  ]
}
```

##### Execution:
```sh
# potoo -c /tmp/potoo_example.json -o /tmp/potoo_example_output.json
```

##### Output:
```sh
# cat /tmp/potoo_example_output.json
{
    "results": [
        {
            "page": "0",
            "results": [
                {
                    "type": "address",
                    "value": "Dr. P.N. Cundall,\nMining Surveys Ltd.,\nHolroyd Road,\nReading,\n\nBerks.\n\n"
                }
            ]
        }
    ]
}
```

### How to build
- Needs at least GCC 4.8 (C++11)
- Install all dependencies
    - Ubuntu 16.04: `sudo apt-get update && sudo apt-get install cmake libboost-all-dev graphicsmagick libgraphicsmagick++1-dev libpoppler-cpp-dev libtbb-dev libtesseract-dev libleptonica-dev`
- Clone the repository: `git clone && cd `
- CMake: `cd build && cmake ..`
- Make: `cd build && make -j4 && sudo make install`
- `potoo --help` should print the help text seen at [Parameters](#parameters)
        
### Todo
- Extend README
- Add better error handling and/or logging
- Add tests!