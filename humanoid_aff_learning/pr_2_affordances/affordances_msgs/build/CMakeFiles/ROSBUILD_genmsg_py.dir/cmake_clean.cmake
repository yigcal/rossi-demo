FILE(REMOVE_RECURSE
  "../msg_gen"
  "../src/affordances_msgs/msg"
  "../msg_gen"
  "CMakeFiles/ROSBUILD_genmsg_py"
  "../src/affordances_msgs/msg/__init__.py"
  "../src/affordances_msgs/msg/_PerceiveRangeData.py"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ROSBUILD_genmsg_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
