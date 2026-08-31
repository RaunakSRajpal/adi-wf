#!/bin/bash
# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# 
# Brief:    House-keeping tasks to keep other build scripts clean
# ------------------------------------------------------------------

if ! (return 2>/dev/null); then
    # if run standalone
    LOGFILE="${1:-$LOGFILE}"
    printf "LOGFILE: $1\t$0\n"
fi


#----------------------------------------------------------
# Logging procedures/functions
#----------------------------------------------------------
usage() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\tusage: $0 <linux-repo dir> <device-tree file> \n" | tee -a $LOGFILE 2>&1
    exit 1
}

error() {
    printf "\n[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    printf "\t\t$2\n\n" | tee -a $LOGFILE >&2
    exit 1
}

status() {
    printf "[INFO]\t$1\n" | tee -a $LOGFILE 2>&1
}

warning() {
    printf "[WARNING]\t$1\n" | tee -a $LOGFILE 2>&1
    printf "\t\t$2\n" | tee -a $LOGFILE 2>&1
}

return_line() {
    printf "\n" | tee -a $LOGFILE 2>&1
}


# ---------------------------------------------------------
# update_env_var:   Updates environment variables inside 
#                   .env files
# usage:
#       update_env_var <env file> <key> <new value>
#----------------------------------------------------------
update_env_var() {
    local env_file="$1"
    local str_regex="$2"
    local set_value="$3"

    # Check if the string passed to regex is valid 
    if [[ ! "$str_regex" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
        error "update_env_var(): Invalid env var key: '${str_regex}'" \
        "env variables can only contain alphanumerics and cannot start with a number (env var: ${env_file}#[${str_regex}])"
        return 1
    fi

    # Count uncommented, non-indented instances of the key (no whitespace around '=')
    local count=$(grep -c "^${str_regex}=" "${env_file}" || true)

    if [[ "$count" -eq 1 ]]; then
        local existing_line=$(grep "^${str_regex}=" "${env_file}")
        local empty_val_regex="^${str_regex}=(\'\'|\"\")?[[:space:]]*(#.*)?\$"
        
        local new_line="${str_regex}=${set_value}"

        if [[ "$existing_line" =~ $empty_val_regex ]]; then
            # Type-1:   key=<empty value>
            # (including "", '', optionally followed by a trailing comment)
            #       -> insert the new value (key=new_val)
            local trailing_comment="${BASH_REMATCH[2]}"

            [ -n "$trailing_comment" ] && \
                new_line="${str_regex}=${set_value} ${trailing_comment}"

            awk -v key="${str_regex}" -v line="${new_line}" '
                $0 ~ "^" key "=" { print line; next }
                { print }
            ' "${env_file}" > "${env_file}.tmp" && mv "${env_file}.tmp" "${env_file}" && \
            status "environment variable updated: ${new_line} \n\t\t \
            (env var: ${env_file}#[${str_regex}])"
        else
            # Type 2:   key=old_val
            # comment out old line, insert new line below 

            # Extract the existing value (strip any trailing comment/whitespace)
            local old_val="${existing_line#${str_regex}=}"
            old_val="${old_val%%#*}"
            old_val="$(echo "$old_val" | sed -e 's/[[:space:]]*$//')"

            if [[ "$old_val" == "$set_value" ]]; then
                status "environment variable '${str_regex}' already set to '${set_value}'. Skipping \n\t\t \
                (env var ${env_file}#[${str_regex}])"
            else
                awk -v key="${str_regex}" -v val="${set_value}" '
                    $0 ~ "^" key "=" { print "# " $0; print key "=" val; next }
                    { print }
                ' "${env_file}" > "${env_file}.tmp" && mv "${env_file}.tmp" "${env_file}" && \
                status "environment variable updated: ${new_line} \n\t\t \
                (env var: ${env_file}#[${str_regex}])"
            fi
        fi
    else
        # Ambiguity (Zero or multiple uncommented instances): just append the "key=value" to the env file
        warning "Ambiguous instances detected. Zero or multiple uncommented instances of the key \'${str_regex}\' were found." \
        "Appending the line: ${new_line} \n\t\t (env var: ${env_file}#[${str_regex}])"
        echo "${str_regex}=${set_value}" >> "${env_file}"
    fi
}
