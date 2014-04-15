#!/bin/sh

# exit if error occured.
set -e

# specify original DAQ-Component name
old_name_camel_case=SampleReader
# specify new DAQ-Component name in Camel (mixed UPPER and lower case)
new_name_camel_case=RawDataReader

# You don't have to rewrite following lines

old_name_lower_case=$(echo $old_name_camel_case | tr '[A-Z]' '[a-z]')
old_name_upper_case=$(echo $old_name_camel_case | tr '[a-z]' '[A-Z]')
new_name_lower_case=$(echo $new_name_camel_case | tr '[A-Z]' '[a-z]')
new_name_upper_case=$(echo $new_name_camel_case | tr '[a-z]' '[A-Z]')

for i in ${old_name_camel_case}.h ${old_name_camel_case}.cpp ${old_name_camel_case}Comp.cpp Makefile; do
    sed -i.bak \
    -e "s/${old_name_lower_case}/${new_name_lower_case}/g" \
    -e "s/${old_name_camel_case}/${new_name_camel_case}/g" \
    -e "s/${old_name_upper_case}/${new_name_upper_case}/g" \
    $i
done

for i in ${old_name_camel_case}.h ${old_name_camel_case}.cpp ${old_name_camel_case}Comp.cpp; do
    old_filename=$i
    new_filename=$(echo $i | sed -e "s/${old_name_camel_case}/${new_name_camel_case}/g")
    mv $old_filename $new_filename
done

rm -f *.bak
