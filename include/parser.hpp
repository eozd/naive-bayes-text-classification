#pragma once

#include <string>
#include <unordered_map>

#include "defs.hpp"

namespace ir {

/**
 * @brief Header that denotes the beginning of a document in Reuters sgm files.
 */
const std::string DOC_HEADER = "<REUTERS";

/**
 * @brief Field inside ir::DOC_HEADER that denotes ID of the document.
 */
const std::string ID_FIELD = "NEWID=\"";

/**
 * @brief Field inside ir::DOC_HEADER that denotes if the document is a
 * train or test document.
 */
const std::string TRAIN_TEST_FIELD = "LEWISSPLIT=\"";

/**
 * @brief ir::TRAIN_TEST_FIELD value denoting the training set.
 */
const std::string TRAIN_KEY = "TRAIN";

/**
 * @brief ir::TRAIN_TEST_FIELD value denoting the test set.
 */
const std::string TEST_KEY = "TEST";

/**
 * @brief Header that denotes the beginning of a topic list.
 */
const std::string TOPIC_HEADER_BEG = "<TOPICS>";

/**
 * @brief Header that denotes the end of a topic list.
 */
const std::string TOPIC_HEADER_END = "</TOPICS>";

/**
 * @brief Tag inside document denoting the beginning of a class keyword.
 */
const std::string CLASS_BEG_TAG = "<D>";

/**
 * @brief Tag inside document denoting the end of a class keyword.
 */
const std::string CLASS_END_TAG = "</D>";

/**
 * @brief Key used to represent ir::DocClass::Earn in Reuters datafiles.
 */
const std::string EARN_CLASS_KEY = "earn";

/**
 * @brief Key used to represent ir::DocClass::Acq in Reuters datafiles.
 */
const std::string ACQ_CLASS_KEY = "acq";

/**
 * @brief Key used to represent ir::DocClass::MoneyFx in Reuters datafiles.
 */
const std::string MONEY_FX_CLASS_KEY = "money-fx";

/**
 * @brief Key used to represent ir::DocClass::Grain in Reuters datafiles.
 */
const std::string GRAIN_CLASS_KEY = "grain";

/**
 * @brief Key used to represent ir::DocClass::Crude in Reuters datafiles.
 */
const std::string CRUDE_CLASS_KEY = "crude";

/**
 * @brief Tag inside document denoting the beginning of document text.
 */
const std::string TXT_BEG_TAG = "<TEXT";

/**
 * @brief Tag inside document denoting the end of document text.
 */
const std::string TXT_END_TAG = "</TEXT";

/**
 * @brief Tag inside document text denoting the beginning of document title.
 */
const std::string TITLE_BEG_TAG = "<TITLE>";

/**
 * @brief Tag inside document text denoting the end of document title.
 */
const std::string TITLE_END_TAG = "</TITLE>";

/**
 * @brief Tag inside document text denoting the beginning of document text body.
 */
const std::string BODY_BEG_TAG = "<BODY>";

/**
 * @brief Tag inside document text denoting the end of document text body.
 */
const std::string BODY_END_TAG = "</BODY>";

/**
 * @brief Parse a Reuters sgm file from the beginning of the given input stream
 * and return a pair of mapping from document IDS to their raw content and
 * document IDs to their types.
 *
 * @param ifs Input stream containing a Reuters sgm file.
 *
 * @return pair of mappings where the first mapping is from document ID to its
 * raw content and the second mapping from document ID to its ir::DocType.
 */
std::pair<raw_doc_index, doc_type_index> parse_file(std::istream& ifs);
} // namespace ir
