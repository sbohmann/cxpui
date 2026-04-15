import * as fs from "node:fs"
import {generateList} from "./generatelist.js";

const outputDirectory = '..'
if (!fs.existsSync(outputDirectory)) {
    throw new RangeError(`Output directory [${outputDirectory}] does not exist`)
}

generateList({
    fileName: 'view_list',
    typeName: 'ViewList',
    elementTypeName: 'View',
    localIncludes: ['cxpui.h'],
    isStruct: true
}, outputDirectory)
