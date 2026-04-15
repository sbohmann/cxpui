import * as fs from "node:fs"
import * as path from "node:path"

export function generateList(definition, outputDirectory){
    let headerPath = path.join(outputDirectory, definition.fileName + '.h')
    fs.writeFileSync(headerPath, headerFileCode())
    let modulePath = path.join(outputDirectory, definition.fileName + '.c')
    fs.writeFileSync(modulePath, moduleFileCode())

    function headerFileCode() {
        return `/* Generated Code */

#ifndef ${includeGuardName()}
#define ${includeGuardName()}

#include <stddef.h>
${includes()}

struct ${definition.typeName};

struct ${definition.typeName} * ${definition.typeName}_create(void);

void ${definition.typeName}_delete(struct ${definition.typeName} *self);

size_t ${definition.typeName}_size(struct ${definition.typeName} const *self);

${elementTypeName()} ${elementAsteriskWithSpace()}${definition.typeName}_get(struct ${definition.typeName} const *self, size_t index);

void ${definition.typeName}_add(struct ${definition.typeName} *self, ${elementTypeName()} ${elementAsterisk()}newElement);

void ${definition.typeName}_print(struct ${definition.typeName} *self);

#endif
`
    }

    function moduleFileCode() {
        return `/* Generated Code */

#include "${definition.fileName}.h"

#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "errors/error.h"

struct ${definition.typeName} {
    ${elementTypeName()} *${elementAsterisk()} data;
    size_t size;
    size_t capacity;
};

struct ${definition.typeName} * ${definition.typeName}_create(void) {
    struct ${definition.typeName} *self = allocate(sizeof(struct ${definition.typeName}));
    *self = (struct ${definition.typeName}) {
        NULL,
        0,
        0
    };
    return self;
}

void ${definition.typeName}_delete(struct ${definition.typeName} *self) {
    free(self->data);
    free(self);
}

size_t ${definition.typeName}_size(struct ${definition.typeName} const *self) {
    return self->size;
}

${elementTypeName()} ${elementAsteriskWithSpace()}${definition.typeName}_get(struct ${definition.typeName} const *self, size_t index) {
    if (index >= self->size) {
        fail_with_message("Index out of bounds [%zu] at size %zu", index, self->size);
    }
    return self->data[index];
}

void ${definition.typeName}_add(struct ${definition.typeName} *self, ${elementTypeName()} ${elementAsterisk()}newElement) {
    if (self->size < self->capacity) {
        self->data[self->size] = newElement;
        ++self->size;
    } else {
        const size_t new_capacity = self->capacity * 3 / 2 + 1;
        ${elementTypeName()} *${elementAsteriskWithSpace()}new_data = allocate(sizeof(${elementTypeName()}${spaceAndElementAsterisk()}) * new_capacity);
        if (self->data != NULL) {
            memcpy(new_data, self->data, sizeof(${elementTypeName()}${spaceAndElementAsterisk()}) * self->size);
            free(self->data);
        }
        new_data[self->size] = newElement;
        ++self->size;
        self->data = new_data;
        self->capacity = new_capacity;
    }
}
`
    }

    function includeGuardName() {
        return definition.fileName.toUpperCase()
    }

    function includes() {
        let result = []
        for (let include of orEmpty(definition.globalIncludes)) {
            result.push(`#include <${include}>`)
        }
        for (let include of orEmpty(definition.localIncludes)) {
            result.push(`#include "${include}"`)
        }
        return result.join('\n')
    }

    function elementTypeName() {
        if (definition.isStruct) {
            return `struct ${definition.elementTypeName}`
        } else {
            return definition.elementTypeName
        }
    }

    function elementAsterisk() {
        return definition.isStruct ? '*' : ''
    }

    function elementAsteriskWithSpace() {
        return definition.isStruct ? '* ' : ''
    }

    function spaceAndElementAsterisk() {
        return definition.isStruct ? ' *' : ''
    }
}

function orEmpty(iterable) {
    if (iterable === undefined) {
        return []
    }
    return iterable
}
