#!/bin/sh

# exit if error occured.
set -e

old_name_camel_case=Sample
new_name_camel_case=RawData

# You don't have to rewrite following lines

old_name_lower_case=$(echo $old_name_camel_case | tr '[A-Z]' '[a-z]')
old_name_upper_case=$(echo $old_name_camel_case | tr '[a-z]' '[A-Z]')
new_name_lower_case=$(echo $new_name_camel_case | tr '[A-Z]' '[a-z]')
new_name_upper_case=$(echo $new_name_camel_case | tr '[a-z]' '[A-Z]')

for i in sample.xml 4comps.xml reader-logger.xml; do
    if [ -f $i ]; then
        sed -i.bak \
        -e "s/${old_name_lower_case}/${new_name_lower_case}/g" \
        -e "s/${old_name_camel_case}/${new_name_camel_case}/g" \
        -e "s/${old_name_upper_case}/${new_name_upper_case}/g" \
        $i
    fi
done

rm -f *.bak
