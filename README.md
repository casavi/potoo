# potoo

potoo is a simple command line tool which takes a pdf file and performs ocr on one or multiple regions per page.  
Highly threaded and performant, with no temporary files being created.  
Ideal for usage in automated systems, e.g. NodeJS or Python scripts.  

Homepage: [casavi.de](http://casavi.de/)  
Author: [Markus Heimbuchner](https://github.com/MHeimbuc)  
Contact us: <hello@casavi.de>

### Parameters
```sh
# ./converter --help

Allowed options:
  --help			        produce this help message

  -c [--config] path		path to config json file (Required)

And ONE of the following:
  -f [--first_page] path	render the first page of the pdf as PNG into <path>
  -h [--human] 				process everything and print to stdout, mainly for debugging
  -o [--output] path		process everything and save it as json at <path>
```

### Config file syntax
Valid JSON file in the following format:
- inputPDF: String, absolute/relative path to input file
- dpi: Int, the DPI to render the PDF with (in PPI)
- language: String, [Tesseract language name](https://github.com/tesseract-ocr/tessdata) - install your wanted packages
- parallel_processing: Bool, if multiple paged pdfs should be processed in parallel or not. Optional, default is single threaded
- crops: Array, multiple regions PER PAGE to OCR
    - type: String, unique name to identify this region later
    - x: Float, percentage distance from the left of the page
    - y: Float, percentage distance from the top
    - w: Float, percentage width of the region
    - h: Float, percentage height of the region

### Hints for Usage
Use the --first_page parameter to get the first page as picture exactly as the tool would if it's run normally.  
Then calculate your region(s) based off this picture and run it with --human for testing.  
If everything went right you should get your text.  

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
- Install all dependencies
    - Ubuntu 16.04: `sudo apt-get update && sudo apt-get install cmake libboost-all-dev graphicsmagick libgraphicsmagick++1-dev libpoppler-cpp-dev libtbb-dev libtesseract-dev libleptonica-dev`
- Clone the repository: `git clone && cd `
- CMake: `cd build && cmake ..`
- Make: `cd build && make -j4 && sudo make install`
- `potoo --help` should print the help text seen at [Parameters](#parameters)
        