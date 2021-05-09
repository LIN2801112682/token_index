# token_index

## 主函数入手：
	* field就是field_dir目录下的对应查询原始数据，调用test_query_group；
	* test_query_group：先把field_dir目录下的所有原始数据文件push，在调用common第二个函数得到query_vec（20条查询）；
	* 重载调用四种test_query方法（参数manager, field, query_vec），并输出
	* test_query：记录开始和结束时间得到查询的总时间，并且通过查询的语句计算输出的平均时间；
	    * 里面对于20条query中的每一个：记录起始和结束时间，得到查询此条query的时间；对manager判断，如果是并，调用manager的retrieve_field_union函数（field_manager中的函数）；是交同理；

## common:
	* line_to_doc:原始数据转成{doc_id, doc, new_line}
	* load_query_vec：把给定的20条query转成小写并存在query_vec中


## field_manager：
	* _field_map：存放field_dir下的每个路径以及使用的索引版本号（map集合）
	* push_field_dir（参数field_dir）：遍历field_dir目录，将目录下的路径和索引版本号存入_field_map；并且创建index，把原始数据 按照types中的数据结构形式 放入数据
	* retrieve_field_union（参数field就是传入的field_dir下的query.txt，参数query就是resource下的query.txt中的20条语句中的一条）：在_field_map集合中查找key值为field对应的value：索引管理版本号，然后调用对应的并操作（如：index_manager_v3_1::retrieve_union）
	* retrieve_field_intersection（参数同理）


## types：三级四级索引 两个交的类型转换为统一的输出结果（doc_id,position,offset），对于三级索引没有offset输出就是（0，0）

## index_manager_v3：
	* inverted_index_v3_t    _inverted_index：（str_t，doc_id_t，position_t）//一个token可在多个doc中，每个doc可能有多个位置position出现
    ```c++
		using position_uset_t = std::unordered_set<position_t>;
    		using doc_id_umap_t = std::unordered_map<doc_id_t, position_uset_t>;
   		using inverted_index_v3_t = std::unordered_map<str_t, doc_id_umap_t>
    ```
	
	* push_col_file、push_doc_line：根据types中的数据结构来把原始数据建立倒排索引
	
	* retrieve_union并：结果unordered_set<doc_id_t> set集合来存放docId（set集合不允许重复，所以并操作不会重复返回）
	
	* retrieve_intersection交：结果vector<doc_id_position_offset_t>